#!/bin/bash

virtualbox_rpm

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
            pm="yum" #yum should proxy to dnf?
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

# Do the virtualbox installation things
if [[ " ${install_list[*]} " =~ virtualbox ]];
then
    echo "Installing Virtualbox"
    case $pm in
        brew)
            echo "brew installing virtualbox"
            ;;
        apt-get)
            echo "Apt-get installing virtualbox"
            sudo echo "deb http://download.virtualbox.org/virtualbox/debian precise contrib" >>  /etc/apt/sources.list
            curl https://www.virtualbox.org/download/oracle_vbox_2016.asc | sudo apt-key add -y -
            sudo apt-get update
            sudo apt-get install -y virtualbox-5.1
            ;;
        yum)
            echo "yum installing virtualbox"
            case $os in
                centos)
                    sudo curl -o /etc/yum.repos.d/vagrant.repo http://download.virtualbox.org/virtualbox/rpm/rhel/virtualbox.repo
                    sudo yum update -y
                    sudo yum install -y VirtualBox-5.1
                    ;;
                fedora)
                    sudo curl -o /etc/yum.repos.d/vagrant.repo http://download.virtualbox.org/virtualbox/rpm/fedora/virtualbox.repo
                    sudo yum update -y
                    sudo yum install -y VirtualBox-5.1
                    ;;
            esac
            ;;
    esac
else
    echo "not installing virtualbox"
fi


#do the virtualbox extension installation things
virtualbox_version=$(VBoxManage --version)
ext_version=$(VBoxManage list extpacks | grep Version | awk '{ print $2}')
echo "vbox v = $virtualbox_version"
echo "ext version = $ext_version"
if [[ -z $ext_version ]]; # && [[ $virtualbox_version =~ .*$ext_version.* ]];
then
    version=$(vboxmanage -v)
    var1=$(echo $version | cut -d 'r' -f 1)
    var2=$(echo $version | cut -d 'r' -f 2)
    file="Oracle_VM_VirtualBox_Extension_Pack-$var1-$var2.vbox-extpack"
    curl http://download.virtualbox.org/virtualbox/$var1/$file -o /tmp/$file
    sudo VBoxManage extpack uninstall "Oracle VM VirtualBox Extension Pack"
    sudo VBoxManage extpack install /tmp/$file --replace
else
    echo "Found a matching Virtualbox and Virtualbox extension pack installation"
fi

#INSTALL VAGRANT
if [[ " ${install_list[*]} " =~ " vagrant " ]];
then
    echo "Installing Vagrant"
    case $pm in
        brew)
            echo "brew installing vagrant"
            ;;
        apt-get)
            echo "apt-get installing vagrant"
            sudo apt-get install -y vagrant
            ;;
        yum)
            echo "yum installing vagrant"
            curl -o vagrant_1.9.4_x86_64.rpm https://releases.hashicorp.com/vagrant/1.9.4/vagrant_1.9.4_x86_64.rpm?_ga=2.56964207.411444733.1494863016-480936336.1491230930
            sudo rpm -Uhv vagrant_1.9.4_x86_64.rpm
            rm vagrant_1.9.4_x86_64.rpm
            echo "Finished installing vagrant"
            ;;
    esac
fi

#finally download the latest vagrant env
echo "Pulling the latest Kubos development environment"
boxes=$(vagrant box list | grep kuboostech/kubos-dev)
if [[ -z $boxes ]];
then
    echo "Adding new box kubostech/kubos-dev"
    vagrant box add kubostech/kubos-dev
else
    echo "Updating kubostech/kubos-dev to the latest version"
    vagrant box update --box kubostech/kubos-dev
fi
