apt-get -y update
apt-get -y install build-essential libssl-dev libffi-dev
apt-get -y install python-setuptools python-dev
apt-get -y install python-pip
add-apt-repository ppa:git-core/ppa
apt-get -y install git
mkdir /home/vagrant/kubos
git clone https://github.com/kubostech/kubos.git /home/vagrant/kubos
