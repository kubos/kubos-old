Getting Started with KubOS RT and the Kubos SDK
===============================================

This is intended to be a quick guide to creating a new KubOS RT project
on top of the Kubos framework.

Prerequisites
-------------

:doc:`Install the Kubos SDK <sdk-installing>`

Create an instance of the Kubos Vagrant box

::

        $ vagrant init kubostech/kubos-dev

SSH into your box

::

        $ vagrant ssh

At this point you will have a new terminal prompt inside your kubos-dev
box.

Creating your Project
---------------------

The simplest way to create a new KubOS RT project is by using the Kubos
CLI. The ``kubos init`` command takes a project name and creates the
project files and folders.

**Note:** Inside of the build system there are several reserved words,
which cannot be used as the name of the project. The most common of
these are ``test``, ``source`` and ``include``.

**Note:** Yotta, the build system the Kubos CLI is based upon, requires
project names to be hyphen-delimited or underscore-delimited. CamelCased
project names will cause warnings.

::

        $ kubos init myproject

The ``init`` command creates a new directory with the
`kubos-rt-example <https://github.com/kubostech/kubos-rt-example>`__
included so you can get started right away.

Cloning a Project
-----------------

We have also created several different example Kubos projects which can
be used as starting points.

-  `Example showing basic FreeRTOS tasks and
   CSP <https://github.com/kubostech/kubos-rt-example>`__
-  `Example showing the I2C HAL and
   sensors <https://github.com/kubostech/kubos-i2c-example>`__
-  `Example showing the SPI HAL and
   sensors <https://github.com/kubostech/kubos-spi-example>`__
-  `Example showing the sensor
   interface <https://github.com/kubostech/kubos-sensor-example>`__
-  `Example showing CSP over
   UART <https://github.com/kubostech/kubos-csp-example>`__
-  `Example KubOS Linux
   project <https://github.com/kubostech/kubos-linux-example>`__

If you would like to use one of our projects, you will need to clone and
link the necessary files. For example:

::

        $ git clone https://github.com/kubostech/kubos-spi-example myproject
        $ cd myproject
        $ kubos link --all

**Note:** It is unnecessary to run the ``kubos init`` command in this
case

Editing the project
-------------------

Whether you have cloned your Kubos project or created it with the
kubos-cli, the default source code entry point is at
``{project directory}/source/main.c``.

There may be additional source files in the
``{project directory}/source`` directory, depending on the specific
project that you are working with. Each of our example applications have
a main.c source file as the entry point of the project.

Choosing a Target
-----------------

Once you have created a project you will need to select a target. The
target defines which hardware your project will run on and how the
peripherals are configured.

You can see a list of available projects by running the following
command:

::

        $ kubos target --list

For this example we will set the msp430f5529 target:

::

        $ kubos target msp430f5529-gcc

For more information on all of the available Kubos targets and selecting
a target see the following **`guide <sdk-cheatsheet>`__**

Building and Flashing
---------------------

Now that the target is set you can begin building. This command will
build the current project:

::

        $ kubos build

You should see the ``Build Succeeded`` message! You are now ready to
load your software on some hardware. Connect your hardware to your
computer and run the following flash command:

::

        $ kubos flash

Congratulations! You have just created a basic Kubos project, built it
and (hopefully) flashed it onto some hardware.
