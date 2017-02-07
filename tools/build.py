#!/usr/bin/env python
from __future__ import print_function
import argparse
import json
import os
import subprocess
import sys
from kubos_build import KubosBuild
import utils

this_dir = os.path.abspath(os.path.dirname(__file__))
root_dir = os.path.dirname(this_dir)

class KubosBuilder(object):
    def __init__(self):
        self.kb = KubosBuild()
        self.modules = self.kb.modules()
        self.targets = self.kb.targets()

    def build(self, module_name="", target_name=""):
        module = next((m for m in self.kb.modules() if m.yotta_name() == module_name), None)
        target = next((t for t in self.kb.targets() if t.yotta_name() == target_name), None)
        if module and target:
            print('Building [module %s@%s] for [target %s] - ' % (module.yotta_name(), module.path, target_name), end="")
            utils.cmd('kubos', 'target', target_name, cwd=module.path, echo=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            utils.cmd('kubos', 'clean', cwd=module.path, echo=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            ret = utils.cmd('yt', 'build', cwd=module.path, echo=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            print('Result %d' % ret)
            return ret
        else:
            if module is None:
                print("Module %s was not found" % module_name)
            if target is None:
                print("Target %s was not found" % target_name)
            return 1

    def list_targets(self):
        for target in self.kb.targets():
            if 'buildTarget' in target.yotta_data:
                print(target.yotta_name())

    def list_modules(self):
        for module in self.kb.modules():
            print(module.yotta_name())

    def find_modules(self, path):
        path_list = path.split("/")
        modules = set()
        # Pop off file name for first directory
        path_list.pop()
        while len(path_list):
            new_path = "/".join(path_list)
            kubos_build = KubosBuild(kubos_dir=new_path)
            for p in kubos_build.projects:
                if p.type != "unknown":
                    modules.add(p.yotta_name())
            if len(modules):
                break

            path_list.pop()
        return modules

    def list_changed_modules(self, ref):
        try:
            git_output = subprocess.check_output(["git", "diff", "--numstat", ref])
            git_lines = [l for l in git_output.splitlines()]
            file_paths = [l.split()[2] for l in git_lines]
            modules = set()
            for path in file_paths:
                modules = modules | (self.find_modules(path))
            
            if len(modules):
                print("Modules changed:")
            for m in modules:
                print(m)
        except subprocess.CalledProcessError:
            print("Error getting changed modules")

    def build_all_targets(self, module_name=""):
        module = next((m for m in self.kb.modules() if m.yotta_name() == module_name), None)
        if module:
            for target in self.kb.build_targets():
                self.build(module.yotta_name(), target.yotta_name())
        else:
            print("Module %s was not found" % module_name)
            return 1

    def build_all_modules(self, target_name=""):
        target = next((t for t in self.kb.targets() if t.yotta_name() == target_name), None)
        if target:
            for module in self.kb.modules():
                self.build(module.yotta_name(), target.yotta_name())
        else:
            print("Target %s was not found" % target_name)
            return 1
    
    def build_all_combinations(self):
        for target in self.kb.targets():
            for module in self.kb.modules():
                self.build(module.yotta_name(), target.yotta_name())

def main():
    parser = argparse.ArgumentParser(
        description='Builds Kubos modules')

    parser.add_argument('--target', metavar='target',
                        help='Specifies target to build modules for')
    parser.add_argument('--module', metavar='module',
                        help='Specifies modules to build')
    parser.add_argument('--all-targets', action='store_true', default=False,
                        help='Builds module for all targets')
    parser.add_argument('--all-modules', action='store_true', default=False,
                        help='Builds all modules for target')
    parser.add_argument('--list-targets', action='store_true', default=False,
                        help='Lists all targets available for building')
    parser.add_argument('--list-modules', action='store_true', default=False,
                        help='Lists all modules found')
    parser.add_argument('--list-changed-modules', action="store", nargs="?", 
                        dest="list_changed_modules", const="HEAD^!",
                        help='Lists modules that have changed. By default will diff against '
                        'the last commit. The git diff path desired can also be passed in')
        

    args = parser.parse_args()

    builder = KubosBuilder()

    ret = 0

    if args.list_targets:
        ret = builder.list_targets()
    elif args.list_modules:
        ret = builder.list_modules()
    elif args.list_changed_modules:
        ret = builder.list_changed_modules(args.list_changed_modules)
    elif args.target and args.module:
        ret = builder.build(module_name=args.module, target_name=args.target)
    elif args.module and args.all_targets:
        ret = builder.build_all_targets(module_name=args.module)
    elif args.target and args.all_modules:
        ret = builder.build_all_modules(target_name=args.target)
    elif args.all_targets and args.all_modules:
        ret = builder.build_all_combinations()
    else:
        parser.print_help()
        ret = -1

    sys.exit(ret)

if __name__ == '__main__':
    main()
