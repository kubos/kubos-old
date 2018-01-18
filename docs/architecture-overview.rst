KubOS Architecture Overview
===========================

The KubOS system is designed to take care of every aspect of the satellite flight software. From the board up, it has every component you need to complete a mission. 


The KubOS Stack
---------------

.. figure:: images/architecture_stack.png
    :align: center


OBC (On Board Computer)
~~~~~~~~~~~~~~~~~~~~~~~

Starting from the bottom, the OBC is up to you, as long as it is in our supported list. The current list of supported OBCs can be found :doc:`here <index>`. We are also continually working to support new platforms, so make sure you talk to us if your OBC is not included! 

Kubos Linux
~~~~~~~~~~~

Kubos Linux is Kubos's Linux build (pretty self explanatory). If you want more information on how it works, what's included, etc, Check it out :doc:`here <os-docs/index>`. 

Kubos APIs
~~~~~~~~~~

There are several Kubos APIs for accomplishing a variety of tasks within Kubos Linux, but the main definition is that they are all statically linked libraries. Anything that behaves this way is considered an API, regardless of what hardware is it interacting with. For example, there are APIs for UART and I2C communication, as well as interaction with specific hardware such as the ISIS iMTQ. You can get more information about what APIs are available and what they do :doc:`here <apis/index>`. 

Kubos Services
~~~~~~~~~~~~~~

The last Kubos portion of the stack is the Kubos Services. These are defined as any persistent process that is used to interact with the satellite. Services rarely make decisions, but will allow the user to accomplish typical Flight Software tasks such as telemetry storage, file management, shell access, hardware interaction, etc. More information about the scope of services can be found :doc:`here <services/index>`. 

Mission Applications
~~~~~~~~~~~~~~~~~~~~

The final portion of the KubOS system is the Mission Applications. These applications are anything that governs the behavior of the satellite. Some common examples are deployment, housekeeping, and telemetry beaconing. Basically anything that you want the satellite to do autonomously goes into this category. More information on and examples of Mission Applications can be found :doc:`here <applications/index>`. 


Typical Mission Architecture
----------------------------

Below is a diagram of a typical mission using KubOS. 

.. figure:: images/mission_diagram.png
    :align: center

In the above diagram, everything in blue is typically Kubos-developed, while everything in green would be mission code and written by the user for their specific mission and payload. 

Communication
~~~~~~~~~~~~~

There is internal communication and external communication, all of which is conducted over different types of IP. 

Gateways
^^^^^^^^

Gateways are any and all communication routes to the satellite. This ranges from desktop serial connection to in flight radio link through a ground station. This is mainly just a term to encompass and abstract the notion of the connection to the satellite. 

Communication Service
^^^^^^^^^^^^^^^^^^^^^

The Communication Service is what integrates the Gateway (and the API developed for it) to talk to the Mission Operates Center, Major Tom. For more information on how it operates, refer to the documentation :doc:`here <services/communication-service>`. 

Hardware Integration
~~~~~~~~~~~~~~~~~~~~

Kubos integrates hardware

Core Services
~~~~~~~~~~~~~

The Core Services listed in the diagram: 

Mission Specific Code
~~~~~~~~~~~~~~~~~~~~~

If you're mostly concerned with "What do I actually need to write?" this section is for you. Things that have to be written no matter 
