#include <stdlib.h>
#define X_MAX 20 /* 그리드 크기 */
#define Y_MAX 20
#define MAX_EPOCHS 100000L /* 에이전트 학습을 위한 이동 횟수 */
#define MAX_ACTIONS 8 /* 8-방향 */
typedef struct{
  double Q[MAX_ACTIONS];
  double sumQ;
  double maxQ;
}stateAction_t;
#define EXPLORE_RATE 0.2
#define LEARNING_RATE 0.6 /* Q-Learning Beta */
#define DISCOUNT_RATE      0.5 /* Q-Learning Lambda */
#define GREEDY 0 /* Choose best Q */
#define P_GREEDY 1 /* 가장 좋은 Q 값을 확률적으로 선택 */
#define getSRand() ((double)rand() / (double)RAND_MAX)
#define getRand(x) (int)((double)(x)*rand()/(RAND_MAX+1.0))
typedef struct{
  int y;
  int x;
 
}loc_t;
const loc_t dir[MAX_ACTIONS] = {
{-1,0}, /* N */
{-1,1}, /* N */
{0,1}, /* N */
{1,1}, /* N */
{1,0}, /* N */
{1,-1}, /* N */
{0,-1}, /* N */  
{-1,-1} /* N */  
};
int grid[Y_MAX][X_MAX] = {
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
  { -1, 1, 1, 1, 1, 1, 1, 1, 1, -1 },
  { -1, 1, 1, 1, 1, 1, 1, 1, 1, -1 },
  { -1, 1, 1, 1, 1, 1, 1, -1, 99, -1 },
  { -1, 1, 1, 1, 1, 1, 1, -1, 9, -1 },
  { -1, 1, 1, 1, 1, 9, 9, -1, 9, -1 },
  { -1, 1, 1, 1, 1, 9, 9, -1, 9, -1 },
  { -1, 1, 1, 1, 1, 9, 9, -1, 9, -1 },
  { -1, -1, 1, 1, 1, 1, 9, 9, 9, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
};
loc_t start = {1, 1};
loc_t end = {3, 8};
stateAction_t stateSpace[Y_MAX][X_MAX];

double explore = EXPLORE_RATE;
int actionSelection = P_GREEDY;
void initStateSpace(void){
  int x_state,y_state,action;
  for(y_state = 0; y_state < Y_MAX; y_state++){
    for(x_state=0; x_state < X_MAX; x_state++){
      for(action =0; action <MAX_ACTIONS; action++){
        stateSpace[y_state][x_state].Q[action] = 0.0;

      }
      stateSpace[y_state][x_state].sumQ = 0.0;
      stateSpace[y_state][x_state].maxQ = 0.0;
    }
  }
  return;
}
void findMaxQ(int y,int x){
  int i ;
  stateSpace[y][x].maxQ = 0.0;
  for(i =0; i < MAX_ACTIONS; i++){
    if(stateSpace[y][x].Q[i] > stateSpace[y][x].maxQ){
      stateSpace[y][x].maxQ = stateSpace[y][x].Q[i];
    }
  }
  return;
}
void updateSum(int y, int x){
  int i;
  stateSpace[y][x].sumQ = 0.0;

  for(i = 0; i < MAX_ACTIONS; i++){
    stateSpace[y][x].sumQ  += stateSpace[y][x].Q[i];
  }
  return;
}
int legalMove(int y_state, int x_state, int action){
  int y = y_state + dir[action].y;
  int x = x_state + dir[action].x;
  if (grid[y][x] < 0) return 0;
  else return  1;

}
int getRandomAction( int y,int x)
{
  int action;
  do{
    action = getRand(MAX_ACTIONS);
  }
  while(!legalMove(y,x,action));
  return action;
  
}
int chooseAction(int y , int x){
  int action;
  int count;
  if(actionSelection == GREEDY){
    findMaxQ(y,x);
    for(action = 0; action < MAX_ACTIONS; action++){
      if(stateSpace[y][x].maxQ == stateSpace[y][x].Q[action]){
        if(legalMove(y,x,action))return(action);

      }

    }
  }
  else if (actionSelection == P_GREEDY) { // 확률을 이용하여 이동
    if ((EXPLORE_RATE > getSRand()) || (stateSpace[y][x].sumQ == 0.0)) {
      action = getRandomAction(y, x); /* Explore(탐사) */
    } 
    else {
      double prob;
      action = getRandomAction(y, x); /* Exploit!(활용)*/
      for (count = 0 ; count < MAX_ACTIONS ; count++) {
        prob = (stateSpace[y][x].Q[action] / stateSpace[y][x].sumQ);
        if (legalMove(y, x, action)) {
          if (prob > getSRand()) {
            return(action);
          }
        }
        if (++action == MAX_ACTIONS) action = 0;
      }
      if (count == MAX_ACTIONS) {
      action = getRandomAction(y, x);
      }
    } // if ((EXPLORE_RATE > getSRand()) || (stateSpace[y][x].sumQ == 0.0)) {
    } else { // if (actionSelection == P_GREEDY)
    //assert(0); // GREEDY 또는 P-GREEDY 가 아니면 종료
    }
    return action;
  }
void updateFunction( int y, int x, int action )
{
  int newy = y + dir[action].y;
  int newx = x + dir[action].x;
  double reward;
  reward = (double)grid[newy][newx] + 0.1;
  /* 상태의 MaxQ 값 갱신 */
  findMaxQ( newy, newx );
  /* Q_Learning( Q-학습: 수식 2.1 수행) */
  stateSpace[y][x].Q[action] +=
  LEARNING_RATE * (reward + (DISCOUNT_RATE *
  stateSpace[newy][newx].maxQ) - stateSpace[y][x].Q[action]);
  /* Update the payoff sum */
  updateSum( y, x );
  return;
}
void showPath( void )
{
int x, y, action;
actionSelection = GREEDY;
x = start.x; y = start.y;
printf("\n");
for ( y = 0 ; y < Y_MAX ; y++ ) {
for ( x = 0 ; x < X_MAX ; x++ ) {
printf("%2d ", grid[y][x]);
}
printf("\n");
}
printf("\n");
x = start.x; y = start.y;
while (1) {
grid[y][x] = 0;
if ((x == end.x) && (y == end.y)) break;
action = chooseAction( y, x );
x += dir[action].x;
y += dir[action].y;
}
printf("\n");
for ( y = 0 ; y < Y_MAX ; y++ ) {
for ( x = 0 ; x < X_MAX ; x++ ) {
printf("%2d ", grid[y][x]);
}
printf("\n");
}
printf("\n");
return;
}
int main(void) {
  long epochs;
  int x, y, action;
  x = start.x; y = start.y;
  srand(time(NULL));
  initStateSpace();
  for ( epochs = 0 ; epochs < MAX_EPOCHS ; epochs++ ) {
  action = chooseAction( y, x );
  updateFunction( y, x, action );
  x += dir[action].x;
  y += dir[action].y;
  if ((x == end.x) && (y == end.y)) {
  x = start.x; y = start.y;
  }
  }
  showPath();
  return 0;
}