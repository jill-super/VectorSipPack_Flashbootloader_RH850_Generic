---
title: "Project Info: CBD1701035"
---

> **Source:** `Doc/DeliveryInformation/Readme_CBD1701035.pdf`  | **Pages:** 11  | **Origin:** Vector Informatik GmbH (SIP — third-party, proprietary license, see delivery documents)

## About this conversion

Project Specific Information for delivery CBD1701035 (11 pages, author Li Wenhe, status Released). Project-specific setup information for the Nexteer EPS bootloader delivery: software tools, documentation list, delivery log, build (compiler/linker options), GENy configuration, demonstration project build and memory map, known issues, and contacts.

Text below was extracted automatically from the PDF; layout, figures and screenshots from the original are not reproduced. For the authoritative version, consult the original PDF in the repository. 

---

### Page 1

Project Specific Information Flash Bootloader © 2018 Vector Informatik GmbH Version 1 based on template version 5.1.0

Flash Bootloader Project Specific Information

## Cbd1701035

Authors Li Wenhe Status Released

### Page 2

Project Specific Information Flash Bootloader © 2018 Vector Informatik GmbH Version 2 based on template version 5.1.0 Contents 1 Introduction................................ ................................ ................................ ................... 3 2 Software Tools ................................ ................................ ................................ .............. 4 3 Documentation ................................ ................................ ................................ ............. 5 4 Delivery Log ................................ ................................ ................................ .................. 6 5 Build ................................ ................................ ................................ .............................. 7 5.1 Compiler and Linker Options ................................ ................................ .................. 7 6 Bootloader Setup ................................ ................................ ................................ .......... 8 6.1 GENy Configuration ................................ ................................ ............................... 8 ### 6.1.1 GENy User Configuration File ................................ ............................ 8 7 Demonstration Project ................................ ................................ ................................ . 9 ### 7.1 Build Demonstration Bootloader/ Application ................................ ......................... 9 7.2 Memory Map ................................ ................................ ................................ .......... 9 8 Known Issues ................................ ................................ ................................ ............. 10 9 Contact ................................ ................................ ................................ ........................ 11

### Page 3

Project Specific Information Flash Bootloader © 2018 Vector Informatik GmbH Version 3 based on template version 5.1.0 1 Introduction This document contains project spe cific information, which cannot be found in any other documentation, like special information to setup the project (if necessary), hints for Demos, restrictions and limitations, common pitfalls and justifications for observed compiler warning and changed project details (i.e. compiler options). This document can restrict descriptions in other documents and contain latest warnings and hints. This delivery contains a FBL_Vector_SLP3 specific bootloader that complies with the OEM flashing specification and flashing sequence.

Caution We have configured the software and programs in accordance with your specifications in the questionnaire. Whereas the programs support other configurations than the one specified in your questionnaire, Vector´s release of the programs delivered to your company is expressly restricted to the configuration you have specified in the questionnaire.

### Page 4

Project Specific Information Flash Bootloader © 2018 Vector Informatik GmbH Version 4 based on template version 5.1.0 2 Software Tools The following Vector tools are included in your Flash Bootloader package. Program Description GENy Code generation tool which auto generates parameter files for the Flash Bootloader based on customer input. vFlash Flash tool which is capable of downloading application as well as calibration data to an ECU running the Flash Bootloader. The tool itself is not a standard part of the delivery and must be purchased separately. The delivery comes with an OEM specific template to be used with the vFlash tool. Hexview Viewer and editor of container files. May be used to edit & create required containers manually. A script solution that generates the required header formats automatically will be provided. Table 2-1 Software tools needed to configure and run the bootloader.

### Page 5

Project Specific Information Flash Bootloader © 2018 Vector Informatik GmbH Version 5 based on template version 5.1.0 3 Documentation The following documentation is included in this delivery:

Note Documentation concerning the Flash Bootloader can be found in the Doc folder of your delivery.

File Description DeliveryDescription_CBD1701035.html Delivery specific information IssueReport_CBD1701035.pdf Known issues at the time of delivery. Readme_CBD1701035.pdf This document. TestReport_FBL_Vector_SLP3.pdf The completed test report. UserManual_FlashBootloader.pdf Getting started with the Flash Bootloader. TechnicalReference_FBL_RH850.pdf Hardware specific Flash Bootloader reference guide. TechnicalReference_FBL_Vector_SLP3.pdf OEM specific Flash Bootloader reference guide. TechnicalReference_NvWrapper.pdf Reference guide for the Nonvolatile memory handler Table 3-1 Documentation List

### Page 6

Project Specific Information Flash Bootloader © 2018 Vector Informatik GmbH Version 6 based on template version 5.1.0 4 Delivery Log Delivery Number Date Details / Updates D00 2018/3/27 Initial delivery

### Page 7

Project Specific Information Flash Bootloader © 2018 Vector Informatik GmbH Version 7 based on template version 5.1.0 5 Build ### 5.1 Compiler and Linker Options The Compiler and Linker Options used in this Bootloader can be found in DeliveryDescription_CBD1701035.html.

### Page 8

Project Specific Information Flash Bootloader © 2018 Vector Informatik GmbH Version 8 based on template version 5.1.0 6 Bootloader Setup ### 6.1 GENy Configuration When creating your own GENy configuration please use the following settings.

Figure 6-1 GENy Configuration ### 6.1.1 GENy User Configuration File A user configuration file is required to properly setup certain parts of the bootloader. The following configurations are required for configuring the bootloader. Required Macro Definition FBL_ENABLE_VECTOR_HW Enable development board support Table 6-1 User Config File Macros

Note An example user configuration file is included with the bootloader demo (.\Demo\DemoFbl\Config\user.cfg)

### Page 9

Project Specific Information Flash Bootloader © 2018 Vector Informatik GmbH Version 9 based on template version 5.1.0 7 Demonstration Project

Caution The demo projects were setup on a local ECU. The configurations as well as the code in the user callback functions are adapted to handle the local setups. Please check configuration and user callbacks carefully before working with the demo projects. The files from _Template folder do not contain the hardware initialization routines. Please refer to the Demo project to get the bootloader running.

### 7.1 Build Demonstration Bootloader/ Application
This delivery comes with a demo for the bootloader and a flashable application. The demo
application is built upon an adapted demo bootloader. Only the files which contain
necessary changes to support the interaction with the bootloader are included (jump from
application into bootloader and reset positive response from application after downloading
process).
The demo comes along with a make support. This system can be used to compile the
demo with just a few modifications on your computer.
Please follow these steps to recompile the demonstration projects:
 Adapt the compiler path. This is done in the file “Demo/DemoFbl/Appl/Makefile“:
COMPILER_BASE = <base folder of your compiler-installation>

Note The compiler path should not have any spaces. If your compiler path is installed in a directory that includes spaces, the entire path must be enclosed in quotes.

 Run “m.bat”. First, a dependency list is generated. If this was successful, the compile and link process is started.  Try “m help” to see a list of all available options of the delivered Make Support. ### 7.2 Memory Map Please reference the linker file provided in the Demo/DemoFbl/Appl directory for an example.

Note Demo/DemoFbl/Appl/DemoFbl.ld

### Page 10

Project Specific Information Flash Bootloader © 2018 Vector Informatik GmbH Version 10 based on template version 5.1.0 8 Known Issues See IssueReport_CBD1701035.pdf.

### Page 11

Project Specific Information Flash Bootloader © 2018 Vector Informatik GmbH Version 11 based on template version 5.1.0 9 Contact Any questions concerning the Flash Bootloader package should be sent to the following email address: support@vector.com

Visit our website for more information on  News  Products  Demo software  Support  Training data  Addresses

www.vector.com

---

[Back to top](#about-this-conversion)
