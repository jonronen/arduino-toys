v 20110115 2
C 40000 40000 0 0 0 title-B.sym
C 51300 45200 1 0 0 resistor-1.sym
{
T 51600 45600 5 10 0 0 0 0 1
device=RESISTOR
T 51400 45500 5 10 1 1 0 0 1
refdes=R1
T 51800 45500 5 10 1 1 0 0 1
value=54.9K
T 51300 45200 5 10 0 0 0 0 1
footprint=1210
}
C 51300 44000 1 0 0 resistor-1.sym
{
T 51600 44400 5 10 0 0 0 0 1
device=RESISTOR
T 51400 44300 5 10 1 1 0 0 1
refdes=R2
T 51900 44300 5 10 1 1 0 0 1
value=10K
T 51300 44000 5 10 0 0 0 0 1
footprint=1210
}
C 48000 42000 1 0 0 ATmega8-1.sym
{
T 48100 46800 5 10 0 0 0 0 1
footprint=32tqfp
T 50300 46600 5 10 1 1 0 6 1
refdes=U1
T 48400 46600 5 10 1 1 0 0 1
device=ATmega328p
}
N 52600 44100 52200 44100 4
N 51300 44100 50600 44100 4
N 52600 43000 52600 46000 4
N 52600 43300 50600 43300 4
N 52600 43000 50600 43000 4
C 48900 49400 1 90 0 crystal-1.sym
{
T 48400 49600 5 10 0 0 90 0 1
device=CRYSTAL
T 49100 49800 5 10 1 1 0 0 1
refdes=U2
T 48200 49600 5 10 0 0 90 0 1
symversion=0.1
T 49600 49700 5 10 1 1 180 0 1
value=16MHz
T 48900 49400 5 10 0 0 0 0 1
footprint=CRYSTAL 300
}
C 46800 49100 1 0 0 gnd-1.sym
C 47300 49900 1 0 0 capacitor-1.sym
{
T 47500 50600 5 10 0 0 0 0 1
device=CAPACITOR
T 47600 50400 5 10 1 1 180 0 1
refdes=C1
T 47500 50800 5 10 0 0 0 0 1
symversion=0.1
T 48300 50400 5 10 1 1 180 0 1
value=22pF
T 47300 49900 5 10 0 0 0 0 1
footprint=1206
}
C 47300 49200 1 0 0 capacitor-1.sym
{
T 47500 49900 5 10 0 0 0 0 1
device=CAPACITOR
T 47600 49200 5 10 1 1 180 0 1
refdes=C2
T 47500 50100 5 10 0 0 0 0 1
symversion=0.1
T 48300 49200 5 10 1 1 180 0 1
value=22pF
T 47300 49200 5 10 0 0 0 0 1
footprint=1206
}
N 47300 50100 46900 50100 4
N 46900 50100 46900 49400 4
N 51000 45300 51300 45300 4
N 52200 45300 52600 45300 4
N 51000 44300 51000 45900 4
N 51000 44300 50600 44300 4
C 52800 42100 1 90 0 capacitor-1.sym
{
T 52100 42300 5 10 0 0 90 0 1
device=CAPACITOR
T 52300 42700 5 10 1 1 180 0 1
refdes=C3
T 51900 42300 5 10 0 0 90 0 1
symversion=0.1
T 52000 42200 5 10 1 1 0 0 1
value=0.1uF
T 52800 42100 5 10 0 0 0 0 1
footprint=1206
}
C 52500 41800 1 0 0 gnd-1.sym
C 56100 44500 1 90 0 connector2-1.sym
{
T 55100 44700 5 10 0 0 90 0 1
device=CONNECTOR_2
T 55900 44400 5 10 1 1 180 0 1
refdes=Sw1
T 56100 44500 5 10 0 0 90 0 1
footprint=CONNECTOR 1 2
}
C 40600 41100 1 0 0 connector2-1.sym
{
T 40800 42100 5 10 0 0 0 0 1
device=CONNECTOR_2
T 40600 41900 5 10 1 1 0 0 1
refdes=Btn1
T 40600 41100 5 10 0 0 0 0 1
footprint=CONNECTOR 1 2
}
C 40600 45500 1 0 0 connector2-1.sym
{
T 40800 46500 5 10 0 0 0 0 1
device=CONNECTOR_2
T 40600 46300 5 10 1 1 0 0 1
refdes=Btn4
T 40600 45500 5 10 0 0 0 0 1
footprint=CONNECTOR 1 2
}
C 40600 44200 1 0 0 connector2-1.sym
{
T 40800 45200 5 10 0 0 0 0 1
device=CONNECTOR_2
T 40600 45000 5 10 1 1 0 0 1
refdes=Btn3
T 40600 44200 5 10 0 0 0 0 1
footprint=CONNECTOR 1 2
}
C 40600 42400 1 0 0 connector2-1.sym
{
T 40800 43400 5 10 0 0 0 0 1
device=CONNECTOR_2
T 40600 43200 5 10 1 1 0 0 1
refdes=Btn2
T 40600 42400 5 10 0 0 0 0 1
footprint=CONNECTOR 1 2
}
C 40600 46700 1 0 0 connector2-1.sym
{
T 40800 47700 5 10 0 0 0 0 1
device=CONNECTOR_2
T 40600 47500 5 10 1 1 0 0 1
refdes=Btn5
T 40600 46700 5 10 0 0 0 0 1
footprint=CONNECTOR 1 2
}
C 56400 42900 1 180 0 connector2-1.sym
{
T 56200 41900 5 10 0 0 180 0 1
device=CONNECTOR_2
T 56400 42100 5 10 1 1 180 0 1
refdes=BtnPwr
T 56400 42900 5 10 0 0 180 0 1
footprint=CONNECTOR 1 2
}
C 54600 41900 1 0 0 gnd-1.sym
N 54700 42400 54700 42200 4
C 55900 47900 1 270 0 connector2-1.sym
{
T 56900 47700 5 10 0 0 270 0 1
device=CONNECTOR_2
T 56100 48000 5 10 1 1 0 0 1
refdes=Batt
T 55900 47900 5 10 0 0 270 0 1
footprint=CONNECTOR 1 2
}
C 56300 45900 1 0 0 gnd-1.sym
C 54700 46800 1 0 0 vcc-1.sym
N 55900 46200 56100 46200 4
C 41600 47700 1 0 0 connector9-1.sym
{
T 41700 51200 5 10 0 0 0 0 1
device=CONNECTOR_9
T 41500 50500 5 10 1 1 180 0 1
refdes=J1
T 41600 47700 5 10 0 0 0 0 1
footprint=CONNECTOR 1 9
}
C 50900 47300 1 270 0 input-2.sym
{
T 51200 46300 5 10 0 1 0 0 1
net=BTN5:1
T 51600 46700 5 10 0 0 270 0 1
device=none
T 51000 46800 5 10 1 1 270 7 1
value=BTN5
}
C 42300 46800 1 0 0 output-2.sym
{
T 43200 46800 5 10 0 1 0 0 1
net=BTN5:1
T 42500 47500 5 10 0 0 0 0 1
device=none
T 43200 46900 5 10 1 1 0 1 1
value=BTN5
}
N 44700 47400 47500 47400 4
N 47500 47400 47500 46300 4
N 47300 46100 48000 46100 4
N 42600 45900 48000 45900 4
N 48000 45700 43000 45700 4
N 42300 44400 44300 44400 4
N 44300 44400 44300 45500 4
N 44300 45500 48000 45500 4
N 48000 45300 44500 45300 4
N 44500 45300 44500 44000 4
C 55900 50200 1 180 0 connector2-1.sym
{
T 55700 49200 5 10 0 0 180 0 1
device=CONNECTOR_2
T 55900 50400 5 10 1 1 180 0 1
refdes=SPKR
T 55900 50200 5 10 0 0 180 0 1
footprint=CONNECTOR 1 2
}
C 54100 48400 1 0 0 gnd-1.sym
C 52200 49100 1 0 0 input-2.sym
{
T 52800 49400 5 10 0 1 0 0 1
net=SPKR:1
T 52800 49800 5 10 0 0 0 0 1
device=none
T 52700 49200 5 10 1 1 0 7 1
value=SPKR
}
C 46300 44600 1 180 0 output-2.sym
{
T 46000 44800 5 10 0 1 180 0 1
net=SPKR:1
T 46100 43900 5 10 0 0 180 0 1
device=none
T 45400 44500 5 10 1 1 180 1 1
value=SPKR
}
N 46300 44500 46300 45100 4
N 46300 45100 48000 45100 4
N 46800 44900 48000 44900 4
N 46900 49400 47300 49400 4
N 44400 43600 48000 43600 4
N 48200 49400 49500 49400 4
N 48200 50100 49500 50100 4
N 42600 45900 42600 45700 4
N 46800 44900 46800 43800 4
N 46800 43800 44200 43800 4
N 44200 42200 44200 43800 4
N 44400 43600 44400 41300 4
N 44400 41300 42300 41300 4
N 44900 43400 48000 43400 4
C 50900 50200 1 180 0 input-2.sym
{
T 50800 50200 5 10 0 1 180 0 1
net=XTL1:1
T 50300 49500 5 10 0 0 180 0 1
device=none
T 50400 50100 5 10 1 1 180 7 1
value=XTL1
}
C 50900 49500 1 180 0 input-2.sym
{
T 50800 49500 5 10 0 1 180 0 1
net=XTL2:1
T 50300 48800 5 10 0 0 180 0 1
device=none
T 50400 49400 5 10 1 1 180 7 1
value=XTL2
}
C 45000 41400 1 90 0 input-2.sym
{
T 45000 41900 5 10 0 1 180 0 1
net=BTN2:1
T 44300 42000 5 10 0 0 90 0 1
device=none
T 44900 41900 5 10 1 1 90 7 1
value=BTN2
}
C 46500 40700 1 90 0 input-2.sym
{
T 46700 41200 5 10 0 1 180 0 1
net=MISO:1
T 45800 41300 5 10 0 0 90 0 1
device=none
T 46400 41200 5 10 1 1 90 7 1
value=MISO
}
C 48200 41800 1 270 0 output-2.sym
{
T 48200 40700 5 10 0 1 0 0 1
net=XTL2:1
T 48900 41600 5 10 0 0 270 0 1
device=none
T 48300 40900 5 10 1 1 270 1 1
value=XTL2
}
C 47700 41800 1 270 0 output-2.sym
{
T 47600 40700 5 10 0 1 0 0 1
net=XTL1:1
T 48400 41600 5 10 0 0 270 0 1
device=none
T 47800 40900 5 10 1 1 270 1 1
value=XTL1
}
C 46800 42100 1 270 0 output-2.sym
{
T 46800 40900 5 10 0 1 0 0 1
net=SCLK:1
T 47500 41900 5 10 0 0 270 0 1
device=none
T 46900 41200 5 10 1 1 270 1 1
value=SCLK
}
C 45800 42100 1 270 0 output-2.sym
{
T 45700 40900 5 10 0 1 0 0 1
net=MOSI:1
T 46500 41900 5 10 0 0 270 0 1
device=none
T 45900 41200 5 10 1 1 270 1 1
value=MOSI
}
C 45300 42100 1 270 0 output-2.sym
{
T 45200 41100 5 10 0 1 0 0 1
net=CS:1
T 46000 41900 5 10 0 0 270 0 1
device=none
T 45400 41200 5 10 1 1 270 1 1
value=CS
}
C 42500 42500 1 0 0 output-2.sym
{
T 42800 42200 5 10 0 1 0 0 1
net=BTN2:1
T 42700 43200 5 10 0 0 0 0 1
device=none
T 43400 42600 5 10 1 1 0 1 1
value=BTN2
}
C 43600 49600 1 0 0 output-2.sym
{
T 44500 49700 5 10 0 1 0 0 1
net=MISO:1
T 43800 50300 5 10 0 0 0 0 1
device=none
T 44500 49700 5 10 1 1 0 1 1
value=MISO
}
C 45000 49500 1 180 0 input-2.sym
{
T 45000 49500 5 10 0 1 180 0 1
net=MOSI:1
T 44400 48800 5 10 0 0 180 0 1
device=none
T 44500 49400 5 10 1 1 180 7 1
value=MOSI
}
C 45000 49200 1 180 0 input-2.sym
{
T 45000 49200 5 10 0 1 180 0 1
net=SCLK:1
T 44400 48500 5 10 0 0 180 0 1
device=none
T 44500 49100 5 10 1 1 180 7 1
value=SCLK
}
C 45000 48900 1 180 0 input-2.sym
{
T 44800 48900 5 10 0 1 180 0 1
net=CS:1
T 44400 48200 5 10 0 0 180 0 1
device=none
T 44500 48800 5 10 1 1 180 7 1
value=CS
}
N 44900 43400 44900 42800 4
N 42300 42600 42500 42600 4
N 43300 49700 43600 49700 4
N 43300 49400 43600 49400 4
N 43600 49100 43300 49100 4
N 43300 48800 43600 48800 4
N 48000 43200 45400 43200 4
N 45400 43200 45400 42100 4
N 48000 43000 45900 43000 4
N 45900 43000 45900 42100 4
N 48000 42800 46400 42800 4
N 46400 42800 46400 42100 4
N 48000 42600 46900 42600 4
N 46900 42600 46900 42100 4
N 48000 42400 47800 42400 4
N 47800 42400 47800 41800 4
N 48000 42200 48000 41800 4
N 48000 41800 48300 41800 4
C 45500 49700 1 0 0 gnd-1.sym
N 45600 50000 43300 50000 4
N 43300 50300 45600 50300 4
C 53600 48700 1 0 0 mosfet-with-diode-1.sym
{
T 54500 49200 5 10 0 0 0 0 1
device=NPN_TRANSISTOR
T 54500 49200 5 10 1 1 0 0 1
refdes=Q1
T 53600 48700 5 10 0 0 0 0 1
footprint=SOT23
}
C 45200 47400 1 90 1 mosfet-with-diode-1.sym
{
T 44700 46500 5 10 0 0 90 6 1
device=NPN_TRANSISTOR
T 44300 47200 5 10 1 1 180 2 1
refdes=Q2
T 45200 47400 5 10 0 0 0 0 1
footprint=SOT23
}
C 46600 46900 1 90 1 mosfet-with-diode-1.sym
{
T 46100 46000 5 10 0 0 270 2 1
device=NPN_TRANSISTOR
T 46700 46600 5 10 1 1 0 6 1
refdes=Q3
T 46600 46900 5 10 0 0 0 0 1
footprint=SOT23
}
C 43500 45300 1 90 1 mosfet-with-diode-1.sym
{
T 43000 44400 5 10 0 0 270 2 1
device=NPN_TRANSISTOR
T 42700 45000 5 10 1 1 0 6 1
refdes=Q4
T 43500 45300 5 10 0 0 0 0 1
footprint=SOT23
}
C 43500 43800 1 90 1 mosfet-with-diode-1.sym
{
T 43000 42900 5 10 0 0 270 2 1
device=NPN_TRANSISTOR
T 42400 43800 5 10 1 1 0 2 1
refdes=Q5
T 43500 43800 5 10 0 0 0 0 1
footprint=SOT23
}
C 43300 42200 1 90 1 mosfet-with-diode-1.sym
{
T 42800 41300 5 10 0 0 270 2 1
device=NPN_TRANSISTOR
T 42200 42200 5 10 1 1 0 2 1
refdes=Q6
T 43300 42200 5 10 0 0 0 0 1
footprint=SOT23
}
C 45400 50300 1 0 0 3.3V-plus-1.sym
C 52400 46000 1 0 0 3.3V-plus-1.sym
C 45500 46700 1 90 0 gnd-1.sym
C 43600 41500 1 90 0 gnd-1.sym
C 43800 43100 1 90 0 gnd-1.sym
C 43800 44600 1 90 0 gnd-1.sym
N 43000 45700 43000 45300 4
N 47300 46100 47300 46900 4
N 46100 46900 47300 46900 4
C 47000 46200 1 90 0 gnd-1.sym
N 45600 46300 42300 46300 4
N 42300 46300 42300 46000 4
N 46600 46300 46700 46300 4
N 44500 44000 43000 44000 4
N 42500 44700 42300 44700 4
N 44200 42200 42800 42200 4
N 42300 42900 42500 42900 4
N 42500 42900 42500 43200 4
C 52500 50300 1 0 0 vcc-1.sym
C 54500 43000 1 0 0 vcc-1.sym
N 44200 46800 44000 46800 4
N 44000 46800 44000 47200 4
N 44000 47200 42300 47200 4
N 47500 46300 48000 46300 4
C 55000 44700 1 0 1 LM1117-3.3.sym
{
T 53900 44800 5 10 1 1 0 6 1
device=LM1117-3.3
T 53400 45700 5 10 1 1 0 0 1
refdes=U3
T 54100 45700 5 10 0 1 0 6 1
footprint=SOT223
}
C 53900 44300 1 0 0 gnd-1.sym
N 53200 45300 52600 45300 4
N 54900 46200 55600 46200 4
N 54900 46800 54900 45300 4
N 54000 44700 54000 44600 4
N 54900 45300 54800 45300 4
N 43000 44000 43000 43800 4
N 42600 45700 42300 45700 4
N 54700 43000 54700 42700 4
C 53300 49900 1 0 0 resistor-1.sym
{
T 53600 50300 5 10 0 0 0 0 1
device=RESISTOR
T 53400 50200 5 10 1 1 0 0 1
refdes=R3
T 53900 50200 5 10 1 1 0 0 1
value=8R (optional)
T 53300 49900 5 10 0 0 0 0 1
footprint=1210
}
N 53300 50000 52700 50000 4
N 52700 50000 52700 50300 4
