#!/bin/bash

#TODO: Figure out the sudo permissions things

vagrant_rpm_url="https://releases.hashicorp.com/vagrant/1.9.4/vagrant_1.9.4_x86_64.rpm"
vagrant_rpm="vagrant_1.9.4_x86_64.rpm"

red='\E[31m'
green='\E[32m'

brew="brew"
apt="apt-get"
dnf="dnf"

programs=(
vagrant
virtualbox
)

install_list=()

set_platform() {
    platform=$(uname)
    if [[ $platform == "Darwin" ]];
    then
        os="macos"
        pm="brew"
        programs+=( "brew" )
    elif [[ $platform == "Linux" ]];
    then
        dist_info=$(cat /etc/*-release)
        if [[ $dist_info =~ .*CentOS.* ]];
        then
            os="centos"
            pm="yum" #yum should proxy to dnf?
        elif [[ $dist_info =~ .*Fedora.* ]]; #Fedora and CentOS do things differently
        then
            os="fedora"
            pm="yum" #yum should proxy to dnf on more modern distros
        elif [[ $dist_info =~ .*ubuntu.* ||  $dist_info =~ .*debian.*  ]];
        then
            os="ubuntu"
            pm="apt-get"
            codename=$(lsb_release -a | grep Codename | tail -n 1 | awk '{print $2') #needed for adding the appropriate .deb repo
        else
            os="unknown"
            pm="unknown"
        fi
    else
        os="unknown"
    fi
}

test_installed () {
    if command -v $1 > /dev/null
    then
        printf "${green}dependency $1 => found\n"
    else
        printf "${red}dependency $1 => not found\n" >&2
        install_list+=($1)
    fi
    tput sgr0 #reset to normal text output
}


# Do the actual installation stuff

set_platform

echo "OS is: $os"
echo "PM is: $pm"

for prog in "${programs[@]}"
do
    test_installed $prog
done

if [[ " ${install_list[*]} " =~ brew ]];
then
    echo "Installing homebrew"
    #this installs the xcode command line tools if they're not already
    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
else
    echo "Homebrew is already installed... Skippping installing it."
fi

# Do the VirtualBox installation things
if [[ " ${install_list[*]} " =~ VirtualBox ]];
then
    echo "Installing VirtualBox"
    case $pm in
        brew)
            brew cask install VirtualBox
            ;;
        apt-get)
            sudo echo "deb http://download.VirtualBox.org/VirtualBox/debian precise contrib" >>  /etc/apt/sources.list
            curl https://www.VirtualBox.org/download/oracle_vbox_2016.asc | sudo apt-key add -y -
            sudo apt-get update
            sudo apt-get install -y VirtualBox-5.1
            ;;
        yum)
            case $os in
                centos)
                    sudo curl -o /etc/yum.repos.d/vagrant.repo http://download.VirtualBox.org/VirtualBox/rpm/rhel/VirtualBox.repo
                    sudo yum update -y
                    sudo yum install -y VirtualBox-5.1
                    ;;
                fedora)
                    sudo curl -o /etc/yum.repos.d/vagrant.repo http://download.VirtualBox.org/VirtualBox/rpm/fedora/VirtualBox.repo
                    sudo yum update -y
                    sudo yum install -y VirtualBox-5.1
                    ;;
            esac
            ;;
    esac
else
    echo "Skipping the VirtualBox installation."
fi


#do the VirtualBox extension installation things
VirtualBox_version=$(VBoxManage --version)
ext_version=$(VBoxManage list extpacks | grep Version | awk '{ print $2 }')
#TODO: Fix this if comparison.
if [[ -z $ext_version ]]; # && [[ $VirtualBox_version =~ .*$ext_version.* ]];
then
    version=$(vboxmanage -v)
    maj_version=$(echo $version | cut -d 'r' -f 1)
    build_no=$(echo $version | cut -d 'r' -f 2)
    file="Oracle_VM_VirtualBox_Extension_Pack-$maj_version-$build_no.vbox-extpack"
    curl http://download.VirtualBox.org/VirtualBox/$maj_version/$file -o /tmp/$file
    sudo VBoxManage extpack install /tmp/$file --replace
else
    echo "Found a matching VirtualBox and VirtualBox extension pack installation"
fi

#INSTALL VAGRANT
if [[ " ${install_list[*]} " =~ " vagrant " ]];
then
    echo "Installing Vagrant"
    case $pm in
        brew)
            echo "brew installing vagrant"
            brew cask install vagrant
            ;;
        apt-get)
            echo "apt-get installing vagrant"
            sudo apt-get install -y vagrant
            ;;
        yum)
            echo "yum installing vagrant"
            curl -o $vagrant_rpm $vagrant_rpm_url
            sudo rpm -Uhv vagrant_1.9.4_x86_64.rpm
            rm vagrant_1.9.4_x86_64.rpm
            echo "Finished installing vagrant"
            ;;
    esac
fi

#install the vbox-guest vagrant plugin
plugin_version=$(vagrant plugin list  | grep vagrant-vbguest)
if [[ -z $plugin_version ]];
then
    vagrant plugin install vagrant-vbguest
else
    echo "Found a version of the vbguest Vagrant plugin... Skipping plugin installation."
fi

#finally download the latest vagrant env
echo "Pulling the latest Kubos development environment"
boxes=$(vagrant box list | grep kubostech/kubos-dev)
if [[ -z $boxes ]];
then
    echo "Adding new box kubostech/kubos-dev"
    vagrant box add kubostech/kubos-dev
else
    echo "Updating kubostech/kubos-dev to the latest version"
    vagrant box update --box kubostech/kubos-dev
fi
