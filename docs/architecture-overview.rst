KubOS Architecture Overview
===========================

The KubOS system is designed to take care of every aspect of the satellite flight software. From the board up, it has every component you might need to complete a mission. The stack is shown below:

.. figure:: images/architecture_stack.png
    :align: center



Getting Started
---------------

If you want to make changes to the Kubos code, perhaps for debugging
purposes or to support a new peripheral, you'll first need to clone the
kubos repo and then pass the folder through to your VM:

:doc:`Install the latest version of the Kubos SDK <../installation-docs/sdk-installing>`

Clone the Kubos repo to your host machine.

::

    $ git clone https://github.com/kubos/kubos