#!/bin/bash

vagrant_deb="vagrant_1.9.5_x86_64.deb"
vagrant_deb_url="https://releases.hashicorp.com/vagrant/1.9.5/$vagrant_deb"

vagrant_rpm="vagrant_1.9.5_x86_64.rpm"
vagrant_rpm_url="https://releases.hashicorp.com/vagrant/1.9.5/$vagrant_rpm"

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
    if [[ $platform == "Darwin" ]]; then
        os="macos"
        pm="brew"
        programs+=( "brew" )
    elif [[ $platform == "Linux" ]]; then
        dist_info=$(cat /etc/*-release)
        if [[ $dist_info =~ .*CentOS.* ]]; then
            os="centos"
            pm="yum" #yum should proxy to dnf
            #Fedora and CentOS do things differently
        elif [[ $dist_info =~ .*Fedora.* ]]; then
            os="fedora"
            pm="yum" #yum should proxy to dnf on more modern distros
        elif [[ $dist_info =~ .*Ubuntu.* ]]; then
            os="ubuntu"
            pm="apt-get"
            #needed for adding the appropriate .deb repo
            codename=$(lsb_release -a | grep -i "codename" | tail -n 1 | awk '{ print $2 }')
        elif [[ $dist_info =~ .*debian.* ]]; then
            os="debian"
            pm="apt-get"
            #needed for adding the appropriate .deb repo
            codename=$(lsb_release -a | grep -i "codename" | tail -n 1 | awk '{ print $2 }')
        else
            os="unknown"
        fi
    else
        os="unknown"
    fi
}


test_installed () {
    if command -v $1 > /dev/null; then
        printf "${green}dependency $1 => found\n"
    else
        printf "${red}dependency $1 => not found\n" >&2
        install_list+=($1)
    fi
    tput sgr0 #reset to normal text output
}


# Do the actual installation stuff

set_platform

if [[ $os == "unknown" ]]; then
    echo "Your operating system type is not supported at this time. For more information see the docs <kubos_doc_link here>"
    exit 1
fi


for prog in "${programs[@]}"
do
    test_installed $prog
done


if [[ " ${install_list[*]} " =~ brew ]]; then
    echo "Installing Homebrew"
    #this installs the xCode command line tools if they're not already
    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
fi


# Do the VirtualBox installation things
if [[ " ${install_list[*]} " =~ virtualbox ]]; then
    echo "Installing VirtualBox"
    case $pm in
        brew)
            brew cask install virtualbox
            ;;
        apt-get)
            echo "deb http://download.VirtualBox.org/VirtualBox/$os $codename contrib" \
                |  sudo tee -a /etc/apt/sources.list
            curl https://www.virtualbox.org/download/oracle_vbox_2016.asc \
                | sudo apt-key add -
            sudo apt-get update
            sudo apt-get install -y virtualbox
            ;;
        yum)
            case $os in
                centos)
                    sudo curl -o /etc/yum.repos.d/vagrant.repo \
                        http://download.VirtualBox.org/virtualbox/rpm/rhel/virtualbox.repo
                    ;;
                fedora)
                    sudo curl -o /etc/yum.repos.d/vagrant.repo \
                        http://download.virtualbox.org/virtualbox/rpm/fedora/virtualbox.repo
                    ;;
            esac
            sudo yum update -y
            sudo yum install -y VirtualBox-5.1
            ;;
    esac
else
    echo "Skipping the VirtualBox installation."
fi


#Do the VirtualBox extension installation things
ext_version=$(vboxmanage list extpacks | grep Version | awk '{ print $2 }')
vbox_version=$(vboxmanage -v)

if [[ $vbox_version =~ _Ubuntu ]]; then
    # An ubuntu .deb of virtualbox has a werid string in the version. This removes it
    vbox_version=$(echo "$vbox_version" | sed 's/_Ubuntu//')
    echo "Vboxversion modified: ${vbox_version}"
fi

maj_version=$(echo $vbox_version | cut -d 'r' -f 1)
build_no=$(echo $vbox_version | cut -d 'r' -f 2)

if [[ -z $ext_version ]] && [[ "$ext_version" == "$maj_version" ]]; then
    echo "Found a matching VirtualBox and VirtualBox extension pack installation"
else
    file="Oracle_VM_VirtualBox_Extension_Pack-$maj_version-$build_no.vbox-extpack"
    curl http://download.virtualbox.org/virtualbox/$maj_version/$file -o $file
    sudo vboxmanage extpack install "$file" --replace \
        --accept-license=715c7246dc0f779ceab39446812362b2f9bf64a55ed5d3a905f053cfab36da9e
    rm $file
fi

#INSTALL VAGRANT
if [[ " ${install_list[*]} " =~ " vagrant " ]]; then
    echo "Installing Vagrant"
    case $pm in
        brew)
            echo "brew installing vagrant"
            brew cask install vagrant
            ;;
        apt-get)
            echo "apt-get installing vagrant"
            curl -o $vagrant_deb $vagrant_deb_url
            sudo dpkg -i $vagrant_deb
            sudo apt-get install -f
            rm $vagrant_deb
            ;;
        yum)
            echo "yum installing vagrant: $vagrant_rpm"
            curl -o $vagrant_rpm $vagrant_rpm_url
            sudo rpm -Uhv $vagrant_rpm
            rm $vagrant_rpm
            echo "Finished installing vagrant"
            ;;
    esac
fi


#Install the vbox-guest vagrant plugin
plugin_version=$(vagrant plugin list  | grep vagrant-vbguest)
if [[ -z $plugin_version ]]; then
    vagrant plugin install vagrant-vbguest
else
    echo "Found a version of the vbguest Vagrant plugin... Skipping plugin installation."
fi


#Finally download the latest vagrant box
echo "Pulling the latest Kubos development environment"
boxes=$(vagrant box list | grep kubostech/kubos-dev)
if [[ -z $boxes ]]; then
    echo "Adding new box kubostech/kubos-dev"
    vagrant box add kubostech/kubos-dev
else
    echo "Updating kubostech/kubos-dev to the latest version"
    vagrant box update --box kubostech/kubos-dev
fi

echo "Done installing Kubos dependencies..."

