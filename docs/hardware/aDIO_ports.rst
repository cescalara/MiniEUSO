aDIO ports
==========

The advanced digital I/O ports (aDIO, CN6) of the CPU are used as an interface to the LVPS, in order to control the power to the instrument subsystems. The Port0 8-bit programmable port and Port1 byte programmable port are used for handling these commands. The pinout of this connecter is:

+----------+----------+-----------+----------+----------+------------+
| Pin      | I/O Port | Function  | Pin      | I/O Port | Function   | 
+==========+==========+===========+==========+==========+============+
| **1**    | P0-0     | HK ON     | **2**    | P0-1     | HK OFF     |
+----------+----------+-----------+----------+----------+------------+
| **3**    | P0-2     | ZYNQ ON   | **4**    | P0-3     | ZYNQ OFF   |
+----------+----------+-----------+----------+----------+------------+
| **5**    | P0-4     | CAM ON    | **6**    | P0-5     | CAM OFF    |
+----------+----------+-----------+----------+----------+------------+
| **7**    | P0-6     |           | **8**    | P0-7     |            |
+----------+----------+-----------+----------+----------+------------+
| **9**    |          | STROBE0   | **10**   |          | STROBE1    |
+----------+----------+-----------+----------+----------+------------+
| **11**   | P1-0     | HK CHECK  | **12**   | P1-1     | ZYNQ CHECK |
+----------+----------+-----------+----------+----------+------------+
| **13**   | P1-2     | CAM CHECK | **14**   | P1-3     |            |
+----------+----------+-----------+----------+----------+------------+
| **15**   |          | DGND      | **16**   |          | +5 V       |
+----------+----------+-----------+----------+----------+------------+


