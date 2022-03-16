/*
 This sketch shows to perform vector products and rotate heading (yaw angle) of the estimated orientation.
 */

#include <imuFilter.h>
#include <basicMPU6050.h>       // Library for IMU sensor. See this link: https://github.com/RCmags/basicMPU6050
#include <U8glib.h>

// SSD1306 Fast I2C / TWI
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);

// Sensor fusion
constexpr float GAIN = 0.1;     
// Fusion gain, value between 0 and 1 - Determines response of heading correction with respect to gravity.
imuFilter <&GAIN> fusion;

// Imu sensor
basicMPU6050<> imu;

// Display function:
void printVector( float r[] ) {
  Serial.print( r[0], 2 );
  Serial.print( "," );
  Serial.print( r[1], 2 );
  Serial.print( "," );
  Serial.print( r[2], 2 );
  Serial.println();
}

void draw(void) {
  u8g.firstPage();
  do {
    //u8g.setFont(u8g_font_unifont);
    u8g.setFont(u8g_font_6x13);
    //u8g.setFont(u8g_font_4x6);
    u8g.drawStr( 0, 15, "Hello MPU-6050!");
  } while (u8g.nextPage());
  delay(500);//500 msec
}

void setup() {
  // Initialize filter: 
  fusion.setup( imu.ax(), imu.ay(), imu.az() );     

  // Calibrate imu
  imu.setup();
  imu.setBias();

  Serial.begin(115200);

  // Vector operations:
  float v1[] = { 3, 1, -1 };                    // Input Vector 
  float axis_y[3], axis_z[3];
  
  fusion.getYaxis( true, axis_y );              // Vectors to operate on [global axes] 
  fusion.getZaxis( true, axis_z );              

  fusion.crossProduct( v1, axis_y );            // Cross product: V = V cross R ; Output is stored in V 
  float v2[] = { v1[0], v1[1], v1[2] };         // Store product
  
  fusion.normalizeVector( v1 );                 // Norm: V = V/|V| ; Output is stored in V 
  
  float dot = fusion.dotProduct( v1, axis_z );  // Dot product: Input order does not matter   
                 
  // Rotate heading:
  #define SMALL_ANG false  
                          // Small angle approximation = true 
                          // Exact angle rotation = false 
  fusion.rotateHeading( SMALL_ANG, dot );

/*
  // Display results:
  Serial.print( "y = " ); 
  printVector( axis_y );
  Serial.print( "v2 = " ); 
  printVector( v2 );
  Serial.print( "v1 = " ); 
  printVector( v1 );
  Serial.print( "dot = ");
  Serial.println( dot );
*/

// assign default color value
  if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
    Serial.println("BW");
  }

  draw();

  // Wait for output to be read
  delay(100);
}

char buf[6];
const float pi = 180.0/3.141592;
float tmp;

void loop() {  
  // Update filter:
  fusion.update( imu.gx(), imu.gy(), imu.gz(), imu.ax(), imu.ay(), imu.az() );    

  // Display angles:
  //Serial.print( fusion.pitch() );
  //Serial.print( "," );
  //Serial.print( fusion.yaw() );
  //Serial.print( "," );
  //Serial.print( fusion.roll() );
  //Serial.println();

  u8g.firstPage();//描画開始
  do {
    tmp = fusion.pitch();
    u8g.drawStr( 0, 10, "x:");
    u8g.drawStr( 20, 10, dtostrf(tmp, 5, 3, buf));
    u8g.drawStr( 70, 10, dtostrf((tmp*pi), 5, 2, buf));

    tmp = fusion.yaw();
    u8g.drawStr( 0, 22, "y:");
    u8g.drawStr( 20, 22, dtostrf(tmp, 5, 3, buf));
    u8g.drawStr( 70, 22, dtostrf((tmp*pi), 5, 2, buf));

    tmp = fusion.roll();
    u8g.drawStr( 0, 34, "z:");
    u8g.drawStr( 20, 34, dtostrf(tmp, 5, 3, buf));
    u8g.drawStr( 70, 34, dtostrf((tmp*pi), 5, 2, buf));

    u8g.drawStr( 0, 46, "tmp:");
    u8g.drawStr( 24, 46, dtostrf(imu.temp(), 5, 1, buf));

  } while (u8g.nextPage());//描画終了
  
  delay(20);//20 msec
}
