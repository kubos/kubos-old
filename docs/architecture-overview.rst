KubOS Architecture Overview
===========================

The KubOS system is designed to take care of every aspect of the satellite flight software. From the board up, it has every component you might need to complete a mission. The stack is shown below:

.. figure:: images/architecture_stack.png
    :align: center

Starting from the bottom, the OBC is up to you, as long as it is in our supported list! The current list of supported OBCs can be found :doc:`here <index.rst>`. We are also continually working to support new platforms, so make sure you talk to us if your OBC is not included. 

Kubos Linux Kubos's Linux build (pretty self explanatory). If you want more information on how it works, what's included, etc, Check it out :doc:`here <os-docs/index.rst>`. 

There are several Kubos APIs for accomplishing a variety of tasks within Kubos Linux, but the main definition is that they are all statically linked libraries. Anything that behaves this way is considered an API, regardless of what hardware is it interacting with. For example, there are APIs for UART and I2C communication, as well as interaction with specific hardware such as the ISIS iMTQ. You can get more information about what APIs are available and what they do :doc:`here <apis/index.rst>`. 

The last Kubos portion of the stack is the Kubos Services. These are defined as any persistent process that is used to interact with the satellite. Services rarely make decisions, but will allow the user to accomplish typical Flight Software tasks such as telemetry storage, file management, shell access, hardware interaction, etc. More information about the scope of services can be found :doc:`here <services/index.rst>`. 

The final portion of the KubOS system is the Mission Applications. These are any processes that govern the behavior of the satellite. Some common examples are deployment, housekeeping, telemetry beaconing, automated telemetry log downlinking, etc. Basically anything that you want the satellite to do autonomously goes into this category. More information on and examples of Mission Applications can be found :doc:`here <applications/index.rst>`. 

Typical Mission Architecture
----------------------------



::

    $ git clone https://github.com/kubos/kubos