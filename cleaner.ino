//미세먼지센서 변수 설정
int sensor_led = 12;      // 미세먼지 센서 안에 있는 적외선 led 핀 번호
int dust_sensor = A0;   // 미세먼지 핀 번호, 미세먼지 센서값 출력핀
int sampling = 280;    // 적외선 led를 키고, 센서 값을 읽어 들여 미세먼지를 측정하는 샘플링 시간
int waiting = 40; // delta time, for pulse width of 0.32ms = 0.28ms + 0.04ms
float stop_time = 9680;   // 센서를 구동하지 않는 시간
float dust_value = 0;  // 센서에서 입력 받은 미세먼지 값
float voltage = 0;
float dustDensityug=0;  // ug/m^3 값을 계산


//장애물 감지 기준 값 (0~1023 : 센서값은 가까울수록 커짐)
//기준값을 높일수록 가까이 있는 장애물을 인식하며, 반대로 기준값을 낮출수록 멀리 있는 장애물을 인식한다.
#define OB_R2 200 // 오른쪽 2개
#define OB_R1 200
#define OB_L1 200 // 왼쪽 2개
#define OB_L2 200

//바닥 감지 기준 값 (0~1023 : 센서값은 가까울수록 커짐)
//마찬가지로 기준값을 높일수록 가까운 바닥을, 기준값을 낮출수록 멀리 있는 바닥을 인식한다.
#define GR_R 250
#define GR_L 250


//바퀴 모터 속도(토크) (0~255 : 값 클수록 빠름)
#define motor_R 250
#define motor_L 250

/*
const int LeftFoward = 6;
const int LeftBack = 9;
const int RightFoward = 10;
const int RightBack = 11;

//int motor_L = LeftFoward, LeftBack;
*/


//흡입모터 속도 (0~255 : 값 클수록 빠름) *주의 100이상으로 높히지 말것
#define motor_V 120


//변수. 
//적외선 센서 6개. 왼쪽 2개, 오른쪽 2개, 바닥 2개.
int sensor0 = 0;
int sensor1 = 0;
int sensor2 = 0;
int sensor3 = 0;
int sensor4 = 0;
int sensor5 = 0;

//장애물 인식 오른쪽, 왼쪽
int obstacle_R = 0;
int obstacle_L = 0;

//바닥. 오른쪽, 왼쪽, 아래
int bottom_R = 0;
int bottom_L = 0;
int bottom = 0;


int cnt = 0;
int flag = 0; 

int bef_on_off_flag = 0;
int on_off_flag = 0;
int on_off_cnt = 3;



void setup() 
{
  Serial.begin(9600);     //시리얼 통신

  pinMode(13, OUTPUT);    // LED는 13번 핀에 연결

  delay(3000);            // 처음 전원이 켜지고 3초 동안 대기.
  
  for(int i=0; i<3; i++)  // LED를 3번 반복하면서 전원이 켜졌음을 표시.
  {
    digitalWrite(13, HIGH); 
    delay(100);
    digitalWrite(13, LOW); 
    delay(100);
  }
}

void loop() 
{
  cnt++;                      //카운트 변수 증가. 루프문이 1번 반복할 때마다.
  if((cnt % 5) == 0)          // 만약 카운트 변수가 5로 나눴을 때 나머지가 0과 같다면, 즉 5번 반복한다면,
    flag = random(0,10);      // flag 라는 변수에 랜덤으로 숫자 0~10 중 임의의 정수를 대입한다.
  
  //센서값 읽어들임
  sensor0 = analogRead(1);  //왼2
  sensor1 = analogRead(2);  //오1
  sensor2 = analogRead(3);  //밑왼
  sensor3 = analogRead(4);  //밑오
  sensor4 = analogRead(5);  //왼1
  sensor5 = analogRead(6);  //오2


  
  
  //장애물 판별 
  if((sensor0 < OB_L2) || (sensor4 < OB_L1))   //왼쪽에 센서 두개 중에 하나라도 장애물 감지되면 
    obstacle_L = 1;                            //obstacle_L(왼쪽 장애물 인식) 이라는 변수를 1로 만든다.
  else
    obstacle_L = 0;
     
  if((sensor1 < OB_R1) || (sensor5 < OB_R2))   //오른쪽에 센서 두개 중에 하나라도 장애물 감지되면
    obstacle_R = 1;                            //obstacle_R 이라는 변수를 1로 만든다.
  else
    obstacle_R = 0;




  //바닥 판별
  
  //바닥을 감지하는 센서가 기준값에 따라 각각 bottom_R, bottom_L 이라는 변수를 1로 만든다.
  if(sensor3 < GR_R)
    bottom_R = 1;
  else
    bottom_R = 0;
  if(sensor2 < GR_L)
    bottom_L = 1;
  else
    bottom_L = 0;

  
  if((sensor3 < GR_R) && (sensor2 < GR_L)) // 바닥을 감지하는 센서 둘 다 바닥을 감지하면 
  {
    bottom = 1;                            // bottom 이라는 변수를 1로 만들고

    on_off_cnt = 0;                        // 로봇 전원을 켜둔 상태로 둔다(?)
  }
  else                                     // 그게 아니면, 즉 둘 다 바닥을 감지하지 못했으면, 
  {
    bottom = 0;                            // bottom 이라는 변수를 0으로 만들고

    //on_off_cnt는 로봇을 들고 있다가 바닥에 내려놓으면 0으로 바뀐 뒤, 바닥이 감지될 때마다 1씩 증가한다.
    on_off_cnt ++;  //둘중하나라도 바닥이 없을때, 500,800 또는 1000,800 시간 딜레이를 가짐 
                    //둘다 바닥이 없는게 2번이면 => 2.6초, 3.6초 
    if(on_off_cnt > 10)
      on_off_cnt = 10;
  }


  //on_off

  bef_on_off_flag = on_off_flag; //이전 상태의 on_off_flag 값을 저장해둔다.

  //로봇을 내려놓고 바닥이 감지되면 on_off_cnt가 0으로 바뀌면서 2보다 작아지면 on_off_flag를 1로 만든다.
  if(on_off_cnt > 2)
    on_off_flag = 0;
  else
    on_off_flag = 1;

  if((bef_on_off_flag == 0) && (on_off_flag == 1))  //꺼졌다가 켜질때 한번만 딜레이 동작. 로봇을 들고 있다가 바닥에 내려놓아서 바닥이 감지되었다는 뜻
  {
    delay(3000);

   //LED를 3번 깜빡이고 켬
    for(int i=0; i<3; i++)
    {
      digitalWrite(13, HIGH); 
      delay(100);
      digitalWrite(13, LOW); 
      delay(100);
    }
    
    digitalWrite(13, HIGH); 
    delay(1000);

    
    //흡입모터 켜짐
    analogWrite(3, motor_V);
    analogWrite(5, LOW);

    delay(3000);
  }


  //on_off_flag의 이전값이 1이었다가 0으로 바뀌는 순간 한번만 동작하는 부분.
  //이 뜻은 로봇이 바닥위에 있다가 들어올려진 순간 한번을 의미.
  if((bef_on_off_flag == 1) && (on_off_flag == 0))  //켜졌다가 꺼질때
  {
    ////LED를 3번 깜빡이고 끔
    for(int i=0; i<2; i++)
    {
      digitalWrite(13, HIGH); 
      delay(100);
      digitalWrite(13, LOW); 
      delay(100);
      
      digitalWrite(13, LOW); 
    }
  }




  if(on_off_flag == 1)      // 로봇이 바닥에 내려놓고 있음을 의미
  {  

    
    if(bottom == 1) //바닥 있을때
    {
      if((obstacle_L == 0) && (obstacle_R == 0))   //양쪽다 장애물 없을떄
      {
        forward();
      } 
      if((obstacle_L == 0) && (obstacle_R == 1))   //오른쪽만 장애물 있을때 
      {
        if(flag % 2)
          clockwise();
        else
          c_clockwise();
        delay(300);
      }
      if((obstacle_L == 1) && (obstacle_R == 0))   //왼쪽만 장애물 있을때
      {
        if(flag % 2)
          clockwise();
        else
          c_clockwise();
        delay(300);
      }
      if((obstacle_L == 1) && (obstacle_R == 1))   //양쪽다 장애물 있을때
      {
        if(flag % 2)
          clockwise();
        else
          c_clockwise();
        delay(300);
      }
    }
  
    
    if(bottom == 0) //바닥 없을때
    {
      if((bottom_L == 1) && (bottom_R == 1))  //양쪽다 바닥이 있을때 (사실상 동작안하는 부분)
      {
        forward();
      }
      if((bottom_L == 1) && (bottom_R == 0))  //오른쪽 바닥 없을때 
      {
        backward();
        delay(600);
        c_clockwise();
        delay(800);
      }
      if((bottom_L == 0) && (bottom_R == 1))  //왼쪽 바닥 없을때 
      {
        backward();
        delay(600);
        clockwise();
        delay(800);
      }
      if((bottom_L == 0) && (bottom_R == 0))  //양쪽다 바닥 없을때
      {
        backward();
        delay(1000);
        clockwise();
        delay(800);
      }
    }
  }

  // on_off_flag가 0일 때 동작하는 부분.
  // on_off_flag가 0이라는 것은 바닥 센서가 둘 중 하나라도 바닥을 감지하지 못해서 on_off_cnt가 증가해서 2보다 커짐을 의미.
  // 즉, 로봇이 바닥면에서 동작하다가 들어올리고 나서 루프문이 3번 돌 동안 바닥을 감지하지 못했다는 것이므로 이때는 바퀴모터를 정지시키고 흡입모터를 끄면서 로봇을 완전히 정지시킨다.
  else
  {
    Stop();

    
    //흡입모터 꺼짐
    analogWrite(3, 0);
    analogWrite(5, LOW);
  
  }



//  dust_check();  // 미세먼지 체크
//
//  if(dustDensityug <= 10){
//    delay(3000);
//    motor_speed_1();
//  }
//  else if(dustDensityug <= 20){
//    delay(3000);
//    motor_speed_2();
//  }
//  else if(dustDensityug <= 30){
//    delay(3000);
//    motor_speed_3();
//  }
//  else{
//    delay(3000);
//    motor_speed_4();
//  }



}

  


/*
  //센서값 확인 
  Serial.print(sensor0);
  Serial.print('\t');
  Serial.print(sensor1);
  Serial.print('\t');
  Serial.print(sensor2);
  Serial.print('\t');
  Serial.print(sensor3);
  Serial.print('\t');
  Serial.print(sensor4);
  Serial.print('\t');
  Serial.println(sensor5);
*/

/*
  //장애물 유무 확인 
  Serial.print(obstacle_L);
  Serial.print('\t');
  Serial.println(obstacle_R);
*/

/*
  //바닥 유무 확인 
  Serial.print(bottom_L);
  Serial.print('\t');
  Serial.println(bottom_R);
*/


  



//바퀴 모터의 동작을 함수로 만듬.
//직진할 때에는 양쪽모터 값을 조금 다르게 두어서 로봇이 좀 더 임의로 움직일 수 있게끔 하였음.
void Stop()
{
    analogWrite(10, 0);
    analogWrite(11, LOW);
    
    analogWrite(6, 0);
    analogWrite(9, LOW);
}
void forward()
{
    analogWrite(10, motor_L);
    analogWrite(11, LOW);
    
    analogWrite(6, motor_R);
    analogWrite(9, LOW);
    
}
void backward()
{
    analogWrite(10, LOW);
    analogWrite(11, motor_L);
    
    analogWrite(6, LOW);
    analogWrite(9, motor_R);
    
}
void clockwise() //시계방향, 우회전
{
    analogWrite(10, motor_L);
    analogWrite(11, LOW);
    analogWrite(6, LOW);
    analogWrite(9, motor_R);
    
    delay(500);

    analogWrite(10, motor_L);
    analogWrite(11, LOW);
    analogWrite(6, motor_R);
    analogWrite(9, LOW);

}
void c_clockwise() //반 시계방향, 좌회전
{
    analogWrite(10, LOW);
    analogWrite(11, motor_L);  
    analogWrite(6, motor_R);     //직진에서는 속도차이를 만들어서 직진으로 가지못하게 함
    analogWrite(9, LOW);  

    delay(500);

    analogWrite(10, motor_L);
    analogWrite(11, LOW);
    analogWrite(6, motor_R);
    analogWrite(9, LOW);
}

//void dust_check(){
//  digitalWrite(sensor_led, LOW); //적외선 LED켜기
//  delayMicroseconds(sampling); // 280us = 0.28ms
//  dust_value = analogRead(dust_sensor); // 센서 값 읽어오기
//  delayMicroseconds(waiting);  // 너무 많은 데이터 입력을 피해주기 위해 잠시 멈춰주는 시간. 
//  digitalWrite(sensor_led, HIGH); // LED 끄기
//  delayMicroseconds(stop_time);   // LED 끄고 대기
//  voltage = dust_value * (5.0 / 1023.0); //0~1023센서값을 0~5V로 변환하기
//  dustDensityug = 50 * voltage; // ug/m3로 단위 변환
//
//  //Serial.print("Dust Density [ug/m3] : ");            // 시리얼 모니터에 미세먼지 값 출력
//  //Serial.println(dustDensityug);
//
//  delay(2000);
//
//}
//
//void motor_speed_1()
//{
//    analogWrite(10, LOW);
//    analogWrite(11, motor_L - 10);  
//    
//    analogWrite(6, motor_R - 10);   
//    analogWrite(9, LOW);
//
//}
//
//void motor_speed_2()
//{
//    analogWrite(10, motor_L - 20);
//    analogWrite(11, LOW);
//    
//    analogWrite(6, LOW);
//    analogWrite(9, motor_R - 20);
//}
//void motor_speed_3()
//{
//    analogWrite(10, motor_L - 30);
//    analogWrite(11, LOW);
//    
//    analogWrite(6, LOW);
//    analogWrite(9, motor_R - 30);
//}
//void motor_speed_4()
//{
//    analogWrite(10, motor_L - 40);
//    analogWrite(11, LOW);
//    
//    analogWrite(6, LOW);
//    analogWrite(9, motor_R - 40);
//}
