# Led cube simulates sand flow principle 

### we can use the data from IMU(like mpu6050) with proper control of led cube to achieve the goal of simulating the sand flow.

# Here is the detail steps:
## 1 Get Euler angle data from IMU with DMP(digital motion processer)
#### You can get the demo for esp32 from this [repo](https://github.com/nopnop2002/esp-idf-mpu6050-dmp). We assume that the Euler angle we get is yaw, pitch, roll.

## 2 Get the plane equation from Eular angle
#### Here we need to do some matrix operation. I will just paste the result here currently.Maybe I will upload the detail extrapolation process later~ :)
#### we assume the coefficients of three axes X Y Z are A B C.
```
A = cos(yaw)*cos(roll) + (sin(pitch)*sin(yaw)*cos(roll) - cos(pitch)*sin(roll)) + (sin(yaw)*cos(pitch)*cos(roll) + sin(pitch)*sin(roll));
B = cos(yaw)*sin(roll) + (sin(pitch)*sin(yaw)*sin(roll) + cos(pitch)*cos(roll)) + (sin(yaw)*sin(roll)*cos(pitch) - sin(pitch)*cos(roll));
C = (-1)*sin(yaw) + sin(pitch)*cos(yaw) + cos(pitch)*cos(yaw);
```

## 3 Get the lowest point in every moment with plane equation
#### In order to simplify the operation. We assume that the lowest can only be one of the eight corner of cube. Here are the coordinate data of them:
```
(0,0,0)
(0,0,7)
(0,7,0)
(0,7,7)
(7,0,0)
(7,0,7)
(7,7,0)
(7,7,7)
```
#### Now we can get the plane equations of every corner on the cube:
|corner coordinate| plane equation | D in plane equation(AX+BY+CZ+D=0) |
|-|-|-|
|(0,0,0)|-x-y-z = 0| (0~-10) |
|(0,0,7)|-x-y+z = 7| (7~-3) |
|(0,7,0)|-x+y-z = 7| (7~-3) |
|(0,7,7)|-x+y+z = 14| (14~4) |
|(7,0,0)|x-y-z = 7| (7~-3) |
|(7,0,7)|x-y+z = 14| (14~4) |
|(7,7,0)|x+y-z = 14| (14-4) |
|(7,7,7)|x+y+z = 21| (21~11)|

#### According to this sheet. We get get the lowest point in every moment with the sign bit of A B C.
#### For example: 

For one mement: A <= 0, B > 0, C <= 0, the lowest point should be (0, 7, 0).And it's relative plane equation is `Ax+By+Cz = 7`(Sign bit are already include in A B C).

## 4 Control the led cube
#### With the lowest point and it's relative plane equation, we can light the leds we need to simulate the sand now.
#### Assume the single led control function is `cube_SetXYZ(X, Y, Z, state)`, here is the demo code to control it:
```
        if(A > 0)
                tmp_cnt += 1;
        if(B > 0)
                tmp_cnt += 1;
        if(C > 0)
                tmp_cnt += 1;
        for(int8_t h = (tmp_cnt * 7);h > (tmp_cnt * 7 - 10); h--){
            for(uint8_t i = 0;i < 8; i++)
                for(uint8_t j = 0;j < 8; j++)
                    for(uint8_t k = 0;k < 8; k++) {
                        if(A*i + B*j + C*k >= h) {
                            cube_SetXYZ(i, j, k, 1);
                            led_num++;
                            if(led_num >= 256) {
                                led_num = 0;
                                goto finish;
                            }
                        }
                    }
        }
finish:
```
