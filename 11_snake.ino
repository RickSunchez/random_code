#define TOP_RIGHT 3
#define BOTTOM_LEFT 2

const int SIZE = 4;                           // размер стороны поля (при условии, что поле квадратное)
// ---------- 1   2   3   4
int ROWS[] = {13, 12, 11, 10}; // пины отвечающие за строки в матрице
int COLS[] = {A3, A2, A1, A0}; // пины отвечающие за столбцы в матрице

bool PICTURE[SIZE][SIZE] = {   // массив, отвечащий за состояния светодиодов на матрице
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0}
};

int8_t SNAKE_X = SIZE/2;     // координаты головы змейки
int8_t SNAKE_Y = SIZE/2;     // координаты головы змейки

uint8_t TAIL_X[SIZE*SIZE];   // массив координат хвоста змейки по оси X
uint8_t TAIL_Y[SIZE*SIZE];   // массив координат хвоста змейки по оси Y
uint8_t TAIL_MAX_LENGTH = 0; // текущая максимальная длина змейки

int8_t DX=0, DY=0;           // направления движения змейки dx: -1(влево), 0(стоп), 1(вправо); dy: -1(вверх), 0(стоп), 1(вправо)

int8_t FOOD_X;               // координаты еды
int8_t FOOD_Y;               // координаты еды
bool FOOD_DRAW = false;      // рисуется ли еда на карте


int FRAME_DELAY = 300; // задержка между перерисовкой кадров (скорость игры)

void setup() {
  for (int i=0; i<SIZE; i++) {
    pinMode(ROWS[i], OUTPUT);
    pinMode(COLS[i], OUTPUT);
  }

  pinMode(TOP_RIGHT, INPUT_PULLUP);
  pinMode(BOTTOM_LEFT, INPUT_PULLUP);

  Serial.begin(9600);
  randomSeed(analogRead(A0));
}

unsigned long last_frame_time = 0;

void loop() {
  if (millis() - last_frame_time > FRAME_DELAY) {
    last_frame_time = millis();
    snake_update();
  }
  
  readButtons();
  
  draw_image(PICTURE);
}

void snake_update() {
  bool STOP = (DX==0 && DY==0);
  
  SNAKE_X += DX;
  SNAKE_Y += DY;

  if (SNAKE_X < 0) SNAKE_X = SIZE - 1;
  if (SNAKE_X > SIZE - 1) SNAKE_X = 0;

  if (SNAKE_Y < 0) SNAKE_Y = SIZE - 1;
  if (SNAKE_Y > SIZE - 1) SNAKE_Y = 0;

  clear_image();
  tail_change();
  
  PICTURE[ SNAKE_Y ][ SNAKE_X ] = 1;
  for (int i=0; i<TAIL_MAX_LENGTH; i++) {
    PICTURE[ TAIL_Y[i] ][ TAIL_X[i] ] = 1;
    
    if (TAIL_X[i] == SNAKE_X && TAIL_Y[i] == SNAKE_Y) {
      TAIL_MAX_LENGTH = 0;
      SNAKE_X = SIZE/2;
      SNAKE_Y = SIZE/2;
      DX=0;
      DY=0;
      return 0;
    }
  }

  if (FOOD_DRAW) {
    PICTURE[ FOOD_Y ][ FOOD_X ] = 1;
    if ( (SNAKE_X == FOOD_X) && (SNAKE_Y == FOOD_Y) ) {
      FOOD_X = random(SIZE);
      FOOD_Y = random(SIZE);
      
      TAIL_MAX_LENGTH++;
      
      Serial.println(TAIL_MAX_LENGTH);
    }
  }
}

void tail_change() {
  for (int i=0; i<TAIL_MAX_LENGTH; i++) {
    TAIL_X[i] = TAIL_X[i+1];
    TAIL_Y[i] = TAIL_Y[i+1];
  }

  TAIL_X[TAIL_MAX_LENGTH] = SNAKE_X;
  TAIL_Y[TAIL_MAX_LENGTH] = SNAKE_Y;
}

bool tr_last_state = false;
bool bl_last_state = false;
void readButtons() { // проверяем какая кнопка нажата и переключаем переменные для смены направления движения
  bool tr = !digitalRead(TOP_RIGHT);
  bool bl = !digitalRead(BOTTOM_LEFT);
  bool STOP = (DX==0 && DY==0); // Проверяем, есть ли движение в текущий момент
  
  if (tr && !tr_last_state) {            // если нажата кнопка TOP_RIGHT...
    if (DX != 0) {        // ...и движение вправо или влево, переключаемся на движение вверх
      DY = -1; DX = 0;  
    } else 
    if (DY != 0) {        // ...или движение вверх или вниз, переключаемся на движение вправо
      DY = 0; DX = 1;  
    }

    if (STOP) DX = 1;  // ...и движения нет, то запускаем движение вправо
  }

  if (bl && !bl_last_state) {            // если нажата кнопка BOTTOM_LEFT...
    if (DX != 0) {       // ...и движение влево или вправо, переключаемся на движение вниз
      DY = 1; DX = 0;
    } else 
    if (DY != 0) {       // ...и движение вниз или вверх, переключаемся на движение влево
      DY = 0; DX = -1;  
    }

    if (STOP) DX = -1; // ...и движения нет, то запускаем движение влево
  }

  if (STOP) {
    FOOD_DRAW = false;
  } else
  if (!STOP && FOOD_DRAW == false){
    FOOD_X = random(SIZE);
    FOOD_Y = random(SIZE);
    FOOD_DRAW = true;
  }

  tr_last_state = tr;
  bl_last_state = bl;
}

void clear_image() {
  for (int y=0; y<SIZE; y++) {
    for (int x=0; x<SIZE; x++) {
      PICTURE[y][x] = 0;
    }  
  }
}

void draw_image(bool image[SIZE][SIZE]) {
  for (int y=0; y<SIZE; y++) {
    digitalWrite(ROWS[y], LOW);
    
    for (int x=0; x<SIZE; x++) {
      digitalWrite(COLS[x], image[y][x]);  
    }
    delay(1);
    clear_matrix();
  }  
}

void clear_matrix() {
  for (int i=0; i<SIZE; i++) {
    digitalWrite(ROWS[i], HIGH); 
    digitalWrite(COLS[i], LOW); 
  }
}
