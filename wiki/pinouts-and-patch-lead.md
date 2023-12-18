 In order to build the patch lead you are going to need the following parts.

* ISO female connector
* Quadlock connector
* Peugeot screen connector (12 pin)
* JST XH 6 type connector
* Some wires

I have included the pinouts which needs to be connected to each other and also drawn a schematics which pins need to be connected if you are more like visual. Also uploaded a photo of such a patch cable so you can have an idea what I've used. 

The images can be found in the repository.

## All together

![iso_quadlock_mfd_bridge](https://github.com/morcibacsi/PSAVanCanBridge/raw/master/images/iso_quadlock_mfd_bridge.png)

## ISO connector pinout

<table>
  <tr>
    <td rowspan="9"><img align="left" width="200" src="https://github.com/morcibacsi/PSAVanCanBridge/raw/master/images/wiki/iso_connector.png"></td>
    <td></td>
    <td></td>
    <td></td>
  </tr>
  <tr>
    <td>B1</td>
    <td>+ Rear Right speaker</td>
    <td>A1</td>
    <td></td>
  </tr>
  <tr>
    <td>B2</td>
    <td>- Rear Right speaker</td>
    <td>A2</td>
    <td>VAN Data bus (9004, DATA, VAN Low)</td>
  </tr>
  <tr>
    <td>B3</td>
    <td>+ Front Right speaker</td>
    <td>A3</td>
    <td>VAN Data bus B (9005, DATAB, VAN High)</td>
  </tr>
  <tr>
    <td>B4</td>
    <td>- Front Right speaker</td>
    <td>A4</td>
    <td>VAN +12V</td>
  </tr>
  <tr>
    <td>B5</td>
    <td>+ Front Left speaker</td>
    <td>A5</td>
    <td>Power for electric aerial (remote +12V for amplifier)</td>
  </tr>
  <tr>
    <td>B6</td>
    <td>- Front Left speaker</td>
    <td>A6</td>
    <td>+12V (switched on ignition)</td>
  </tr>
  <tr>
    <td>B7</td>
    <td>+ Rear Left speaker</td>
    <td>A7</td>
    <td>+12V (permanent)</td>
  </tr>
  <tr>
    <td>B8</td>
    <td>- Rear Left speaker</td>
    <td>A8</td>
    <td>GND</td>
  </tr>
</table>

## Display connector pinout (12 pin)

<table>
  <tr>
    <td rowspan="13"><img align="left" height="200" src="https://github.com/morcibacsi/PSAVanCanBridge/raw/master/images/wiki/can_display_connector.png"></td>
    <td></td>
  </tr>
  <tr>
    <td>1</td>
    <td>+12V</td>
  </tr>
  <tr>
    <td>2</td>
    <td></td>
  </tr>
  <tr>
    <td>3</td>
    <td>GND</td>
  </tr>
  <tr>
    <td>4</td>
    <td></td>
  </tr>
  <tr>
    <td>5</td>
    <td></td>
  </tr>
  <tr>
    <td>6</td>
    <td></td>
  </tr>
  <tr>
    <td>7</td>
    <td>CAN Low (9025)</td>
  </tr>
  <tr>
    <td>8</td>
    <td></td>
  </tr>
  <tr>
    <td>9</td>
    <td>CAN High (9024)</td>
  </tr>
  <tr>
    <td>10</td>
    <td></td>
  </tr>
  <tr>
    <td>11</td>
    <td></td>
  </tr>
  <tr>
    <td>12</td>
    <td></td>
  </tr>
</table>

## Display connector pinout (6 pin)

<table>
  <tr>
    <td rowspan="13"><img align="left" height="200" src="https://github.com/morcibacsi/PSAVanCanBridge/raw/master/images/wiki/can_display_connector_6_pin.png"></td>
    <td></td>
  </tr>
  <tr>
    <td>1</td>
    <td>+12V</td>
  </tr>
  <tr>
    <td>2</td>
    <td></td>
  </tr>
  <tr>
    <td>3</td>
    <td>GND</td>
  </tr>
  <tr>
    <td>4</td>
    <td>CAN High (9024)</td>
  </tr>
  <tr>
    <td>5</td>
    <td></td>
  </tr>
  <tr>
    <td>6</td>
    <td>CAN Low (9025)</td>
  </tr>
</table>

## Matrix display (MATT) connector pinout (6 pin)

<table>
  <tr>
    <td rowspan="13"><img align="left" height="200" src="https://github.com/morcibacsi/PSAVanCanBridge/raw/master/images/wiki/can_display_connector_6_pin.png"></td>
    <td></td>
  </tr>
  <tr>
    <td>1</td>
    <td>+12V</td>
  </tr>
  <tr>
    <td>2</td>
    <td>CAN Low</td>
  </tr>
  <tr>
    <td>3</td>
    <td>CAN High</td>
  </tr>
  <tr>
    <td>4</td>
    <td></td>
  </tr>
  <tr>
    <td>5</td>
    <td></td>
  </tr>
  <tr>
    <td>6</td>
    <td>GND</td>
  </tr>
</table>

## Quadlock connector pinout

<table>
    <tr>
        <td rowspan="9"><img align="left" width="200" src="https://github.com/morcibacsi/PSAVanCanBridge/raw/master/images/wiki/quadlock_connector.jpg"></td>
        <td></td>
        <td></td>
        <td></td>
    </tr>
    <tr>
        <td>A1</td>
        <td>+ Rear Right speaker</td>
        <td>A9</td>
        <td></td>
    </tr>
    <tr>
        <td>A2</td>
        <td>+ Front Right speaker</td>
        <td>A10</td>
        <td>CAN High (9024)</td>
    </tr>
    <tr>
        <td>A3</td>
        <td>+ Front Left speaker</td>
        <td>A11</td>
        <td>Power for electric aerial (remote +12V for amplifier)</td>
    </tr>
    <tr>
        <td>A4</td>
        <td>+ Rear Left speaker</td>
        <td>A12</td>
        <td>+12V</td>
    </tr>
    <tr>
        <td>A5</td>
        <td>- Rear Right speaker</td>
        <td>A13</td>
        <td>CAN Low (9025)</td>
    </tr>
    <tr>
        <td>A6</td>
        <td>- Front Right speaker</td>
        <td>A14</td>
        <td></td>
    </tr>
    <tr>
        <td>A7</td>
        <td>- Front Left speaker</td>
        <td>A15</td>
        <td></td>
    </tr>
    <tr>
        <td>A8</td>
        <td>- Rear Left speaker</td>
        <td>A16</td>
        <td>GND</td>
    </tr>
</table>
----

## JST XH6 connector pinout

<table>
    <tr>
        <td rowspan="9"><img align="left" width="200" src="https://github.com/morcibacsi/PSAVanCanBridge/raw/master/images/wiki/jst_xh6.png"></td>
        <td></td>
    </tr>
    <tr>
        <td>1</td>
        <td>VAN DATAB</td>
    </tr>
    <tr>
        <td>2</td>
        <td>VAN DATA</td>
    </tr>
    <tr>
        <td>3</td>
        <td>CAN Low</td>
    </tr>
    <tr>
        <td>4</td>
        <td>CAN High</td>
    </tr>
    <tr>
        <td>5</td>
        <td>GND</td>
    </tr>
    <tr>
        <td>6</td>
        <td>+12V</td>
    </tr>
</table>

## The lead

![components](https://github.com/morcibacsi/PSAVanCanBridge/raw/master/images/components.png)
