/*
 * FILE: lcdlib.c
 * 实现TFT LCD的测试函数
 */
#include "lcdlib.h"
#include "serial.h"
extern void delay(int us);
extern volatile int led_flag1;
// extern volatile int led_flag2;
// extern volatile int led_flag3;
extern volatile int game_flag1;
extern volatile int state;



extern const unsigned char gImage_bulb_100_100[20000];
extern const unsigned char gImage_bulb_yellow_100_100[20000];
extern const unsigned char gImage_key_100_50[10000];
extern const unsigned char gImage_key_pink_100_50[10000];
extern const unsigned char gImage_exit_50_50[5000];

// 游戏图片
extern const unsigned char gImage_background_20_20[800];
extern const unsigned char gImage_box_blue_20_20[800];
extern const unsigned char gImage_box_red_20_20[800];
extern const unsigned char gImage_brick_20_20[800];
extern const unsigned char gImage_player_20_20[800];
extern const unsigned char gImage_destination_20_20[800];
extern const unsigned char gImage_up_50_50[5000];
extern const unsigned char gImage_down_50_50[5000];
extern const unsigned char gImage_left_50_50[5000];
extern const unsigned char gImage_right_50_50[5000];;
extern const unsigned char gImage_restart_50_50[5000];
extern const unsigned char gImage_nextlevel_50_50[5000];
extern const unsigned char gImage_next_180_90[32400];
extern const unsigned char gImage_gameover_200_100[40000];

// 定义游戏区域
#define game_x1 0
#define game_x2 479
#define game_y1 0
#define game_y2 199 
// 游戏区重新定义了坐标，由于所有元素都是20x20的大小，故以20为坐标的尺度单位


extern void delay(int us);

void ShowPicture(UINT32 x, UINT32 y, UINT32 x_size, UINT32 y_size, unsigned char* pic)
{
    //printf("开始画图\n");
    int i = 0,j = 0, k = 0;
    for(i = y; i < y + y_size; i++)
    {
        for(j = x; j < x + x_size ; j++)
        {
            DrawPixel(j,i,pic[k],pic[k+1]);
            k = k+2;
        }
    }
    //printf("画图完成\n");
}

void Lcd_Led(void)
{
    ShowPicture(20,70,100,100,gImage_bulb_100_100);
    ShowPicture(180,70,100,100,gImage_bulb_100_100);
    ShowPicture(340,70,100,100,gImage_bulb_100_100);
    ShowPicture(20,210,100,50,gImage_key_100_50);
    ShowPicture(180,210,100,50,gImage_key_100_50);
    ShowPicture(340,210,100,50,gImage_key_100_50);
    ShowPicture(429,0,50,50,gImage_exit_50_50);

    while(1)
    {
        // 可以进来
        if(led_flag1 == 1)
        {
            ShowPicture(20,70,100,100,gImage_bulb_yellow_100_100);
            ShowPicture(180,70,100,100,gImage_bulb_yellow_100_100);
            ShowPicture(340,70,100,100,gImage_bulb_yellow_100_100);
            ShowPicture(20,210,100,50,gImage_key_pink_100_50);
            ShowPicture(180,210,100,50,gImage_key_100_50);
            ShowPicture(340,210,100,50,gImage_key_100_50);

        }
        else if(led_flag1 == 2)
        {
            ShowPicture(20,70,100,100,gImage_bulb_100_100);
            ShowPicture(180,70,100,100,gImage_bulb_100_100);
            ShowPicture(340,70,100,100,gImage_bulb_100_100);
            ShowPicture(20,210,100,50,gImage_key_100_50);
            ShowPicture(180,210,100,50,gImage_key_pink_100_50);
            ShowPicture(340,210,100,50,gImage_key_100_50); 
        }
        else if(led_flag1 == 3)
        {
            ShowPicture(20,210,100,50,gImage_key_100_50);
            ShowPicture(180,210,100,50,gImage_key_100_50);
            ShowPicture(340,210,100,50,gImage_key_pink_100_50);

            //左
            if(led_flag1 != 3) continue;
            ShowPicture(20,70,100,100,gImage_bulb_yellow_100_100);
            delay(1);
            ShowPicture(20,70,100,100,gImage_bulb_100_100);

            //中
            if(led_flag1 != 3) continue;
            ShowPicture(180,70,100,100,gImage_bulb_yellow_100_100);
            delay(1);
            ShowPicture(180,70,100,100,gImage_bulb_100_100);

            //右
            if(led_flag1 != 3) continue;
            ShowPicture(340,70,100,100,gImage_bulb_yellow_100_100);
            delay(1);
            ShowPicture(340,70,100,100,gImage_bulb_100_100);
        }
        if(state == 0)
        {
            break;
        }

    }
}


/* ----------------------------------------------以下是游戏部分----------------------------------------------------------- */

struct GameElem
{
    int locate_arr[2][40];   // 存放元素的坐标
    int num;                 // 元素的个数
    unsigned char* picture;  // 图片资源指针
};

// 全局循环变量
int i = 0, j = 0, k = 0, l = 0, p = 0;
// 全局标志量
int game_flag2_box = 0;
int game_flag3_plug = 1;
int game_win = 0;

// 墙壁
struct GameElem brick;
// 蓝色箱子
struct GameElem box_blue;
// 人物
struct GameElem player;
// 目的地
struct GameElem destination;

// 关卡1 数据保存
int brick_1[2][22] = {{9,10,11,12,9,12,9,12,9,12,13,14,9,14,9,14,9,10,11,12,13,14},{2,2,2,2,3,3,4,4,5,5,5,5,6,6,7,7,8,8,8,8,8,8}};
int box_blue_1[2][2] = {{11,12},{6,6}};
int player_1[2][1] = {{11},{5}};
int destination_1[2][2] = {{10,13},{7,7}};

// 关卡2 数据保存
int brick_2[2][28] = {{8,9,10,11,12,8,12,13,8,13,14,15,8,9,15,9,15,9,13,14,15,9,13,9,10,11,12,13},\
                      {1,1,1,1,1,2,2,2,3,3,3,3,4,4,4,5,5,6,6,6,6,7,7,8,8,8,8,8}};
int box_blue_2[2][5] = {{11,11,11,10,12},{3,4,5,6,6}};
int player_2[2][1] = {{13},{5}};
int destination_2[2][5] = {{11,11,11,11,11},{3,4,5,6,7}};



/*-----------------------------------------------------游戏函数部分----------------------------------------------------------*/



void ShowElem(struct GameElem e)
{
    int i = 0;
    for(i = 0; i< e.num; i++)
    {
        ShowPicture(e.locate_arr[0][i]*20, e.locate_arr[1][i]*20, 20, 20, e.picture);
    }
}

void ShowBack(int locateX, int locateY)
{
    ShowPicture(locateX*20, locateY*20, 20, 20, gImage_background_20_20);
}

void load_data(int n)
{
    switch(n)
    {
        case 1:
        {
            for(j = 0;j<22;j++)
            {
                brick.locate_arr[0][j] = brick_1[0][j];
                brick.locate_arr[1][j] = brick_1[1][j];
            }
            brick.num = 22;
            brick.picture = gImage_brick_20_20;

            for(j = 0;j<2;j++)
            {
                box_blue.locate_arr[0][j] = box_blue_1[0][j];
                box_blue.locate_arr[1][j] = box_blue_1[1][j];
                destination.locate_arr[0][j] = destination_1[0][j];
                destination.locate_arr[1][j] = destination_1[1][j];
            }
            box_blue.num = 2;
            destination.num = 2;
            box_blue.picture = gImage_box_blue_20_20;
            destination.picture = gImage_destination_20_20;

            player.locate_arr[0][0] = player_1[0][0];
            player.locate_arr[1][0] = player_1[1][0];
            player.num = 1;
            player.picture = gImage_player_20_20;
            
            break;
        }
        case 2:
        {
            for(j = 0;j<28;j++)
            {
                brick.locate_arr[0][j] = brick_2[0][j];
                brick.locate_arr[1][j] = brick_2[1][j];
            }
            brick.num = 28;
            brick.picture = gImage_brick_20_20;

            for(j = 0;j<5;j++)
            {
                box_blue.locate_arr[0][j] = box_blue_2[0][j];
                box_blue.locate_arr[1][j] = box_blue_2[1][j];
                destination.locate_arr[0][j] = destination_2[0][j];
                destination.locate_arr[1][j] = destination_2[1][j];
            }
            box_blue.num = 5;
            destination.num = 5;
            box_blue.picture = gImage_box_blue_20_20;
            destination.picture = gImage_destination_20_20;

            player.locate_arr[0][0] = player_2[0][0];
            player.locate_arr[1][0] = player_2[1][0];
            player.num = 1;
            player.picture = gImage_player_20_20;
            
            break;
        }
    }
}



// 更新箱子函数，只有人物碰到箱子才会调用这个函数
void updata_box(int index,int n)  // index表示是哪个箱子  n表示方向（上下左右）
{
    switch(n)
    {
        case 1:   
        {
            for(k = 0;k<brick.num;k++)  // 如果箱子碰到了墙，表示被堵住
            {
                if( ((box_blue.locate_arr[1][index] - 1)==brick.locate_arr[1][k]) && (box_blue.locate_arr[0][index]==brick.locate_arr[0][k]) )
                {
                    game_flag3_plug = 0;
                    break;
                }
            }
            for(l = 0;l<box_blue.num;l++) // 如果箱子碰到了箱子，表示被堵住
            {
                if( ((box_blue.locate_arr[1][index] - 1)==box_blue.locate_arr[1][l]) && (box_blue.locate_arr[0][index]==box_blue.locate_arr[0][l]) )
                {
                    game_flag3_plug = 0;
                    break;
                }
            } 
            if((k == brick.num) && (l == box_blue.num))   // 如果没有碰到墙壁和箱子，则可以移动
            {
                game_flag3_plug = 1;
                box_blue.locate_arr[1][index] -= 1;
                ShowElem(box_blue);
            }
            break;
        }
        case 2:
        {
            for(k = 0;k<brick.num;k++)      // 如果箱子碰到了墙，不能移动了
            {
                if( ((box_blue.locate_arr[1][index] + 1)==brick.locate_arr[1][k]) && (box_blue.locate_arr[0][index]==brick.locate_arr[0][k]) )
                {
                    game_flag3_plug = 0;
                    break;
                }
            }
            for(l = 0;l<box_blue.num;l++)   // 如果箱子碰到了箱子，不能移动了
            {
                if( ((box_blue.locate_arr[1][index] + 1)==box_blue.locate_arr[1][l]) && (box_blue.locate_arr[0][index]==box_blue.locate_arr[0][l]) )
                {
                    game_flag3_plug = 0;
                    break;
                }
            } 
            if((k == brick.num) && (l == box_blue.num))
            {
                game_flag3_plug = 1;
                box_blue.locate_arr[1][index] += 1;
                ShowElem(box_blue);
            }
            break;
        }
        case 3:
        {
            for(k = 0;k<brick.num;k++)      // 如果箱子碰到了墙，不能移动了
            {
                if( ((box_blue.locate_arr[1][index])==brick.locate_arr[1][k]) && (box_blue.locate_arr[0][index]-1==brick.locate_arr[0][k]) )
                {
                    game_flag3_plug = 0;
                    printf("\n箱子撞到墙了\n");
                    break;
                }
            }
            for(l = 0;l<box_blue.num;l++)   // 如果箱子碰到了箱子，不能移动了
            {
                if( ((box_blue.locate_arr[1][index])==box_blue.locate_arr[1][l]) && (box_blue.locate_arr[0][index]-1==box_blue.locate_arr[0][l]) )
                {
                    game_flag3_plug = 0;
                    printf("\n箱子撞到箱子了\n");
                    break;
                }
            } 
            if((k == brick.num) && (l == box_blue.num))
            {
                game_flag3_plug = 1;
                box_blue.locate_arr[0][index] -= 1;
                ShowElem(box_blue);
            }
            break;
        }
        case 4:
        {
            for(k = 0;k<brick.num;k++)      // 如果箱子碰到了墙，不能移动了
            {
                if( ((box_blue.locate_arr[1][index])==brick.locate_arr[1][k]) && (box_blue.locate_arr[0][index]+1==brick.locate_arr[0][k]) )
                {
                    game_flag3_plug = 0;
                    break;
                }
            }
            for(l = 0;l<box_blue.num;l++)   // 如果箱子碰到了箱子，不能移动了
            {
                if( ((box_blue.locate_arr[1][index])==box_blue.locate_arr[1][l]) && (box_blue.locate_arr[0][index]+1==box_blue.locate_arr[0][l]) )
                {
                    game_flag3_plug = 0;
                    break;
                }
            } 
            if((k == brick.num) && (l == box_blue.num))
            {
                game_flag3_plug = 1;
                box_blue.locate_arr[0][index] += 1;
                ShowElem(box_blue);
            }
            break;
        }

    }



}

// 如何定义被堵住：推箱子时箱子另一边是墙壁或者箱子
void update_player()
{
    int i = 0;
    if(game_flag1 == 1) // 向上走
    {
        game_flag1 = 0;
        // 是否碰到墙壁
        for(i = 0;i<brick.num;i++)
        {
            if( ((player.locate_arr[1][0] - 1)==brick.locate_arr[1][i]) && (player.locate_arr[0][0]==brick.locate_arr[0][i]) )
            {
                break;
            }
        }
        // 是否碰到箱子 
        for(j = 0;j<box_blue.num;j++)
        {
            if( ((player.locate_arr[1][0] - 1)==box_blue.locate_arr[1][j]) && (player.locate_arr[0][0]==box_blue.locate_arr[0][j]) )
            {
                game_flag2_box = 1; // 向上走碰到箱子
                updata_box(j,game_flag2_box);
                break;
            }
        }
        if(j == box_blue.num) game_flag3_plug = 1;          // 没有碰到箱子，没有堵住
        
        if((i == brick.num) && (game_flag3_plug == 1))      //没有碰到墙壁，并且没有被堵住
        {
            // 人物移动之后，其原来的位置由背景板或者目的地代替
            for(j = 0;j<destination.num;j++)
            {
                if( (player.locate_arr[0][0] == destination.locate_arr[0][j]) && (player.locate_arr[1][0] == destination.locate_arr[1][j]) )
                {
                    ShowPicture(destination.locate_arr[0][j]*20, destination.locate_arr[1][j]*20, 20, 20, destination.picture);
                    break;
                }
            }
            if(j == destination.num) ShowBack(player.locate_arr[0][0],player.locate_arr[1][0]); 
            
            player.locate_arr[1][0] -= 1;
            ShowElem(player);
            
        }
    }
    else if(game_flag1 == 2) // 向下走
    {
        game_flag1 = 0;
        for(i = 0;i<brick.num;i++)
        {
            if( ((player.locate_arr[1][0] + 1)==brick.locate_arr[1][i]) && (player.locate_arr[0][0]==brick.locate_arr[0][i]) )
            {
                break;
            }
        }
        for(j = 0;j<box_blue.num;j++)
        {
            if( ((player.locate_arr[1][0] + 1)==box_blue.locate_arr[1][j]) && (player.locate_arr[0][0]==box_blue.locate_arr[0][j]) )
            {
                game_flag2_box = 2; // 向下走碰到箱子
                updata_box(j,game_flag2_box);
                break;
            }
        }
        if(j == box_blue.num) game_flag3_plug = 1;
        if((i == brick.num) && (game_flag3_plug == 1))
        {
            for(j = 0;j<destination.num;j++)
            {
                if( (player.locate_arr[0][0] == destination.locate_arr[0][j]) && (player.locate_arr[1][0] == destination.locate_arr[1][j]) )
                {
                    ShowPicture(destination.locate_arr[0][j]*20, destination.locate_arr[1][j]*20, 20, 20, destination.picture);
                    break;
                }
            }
            if(j == destination.num) ShowBack(player.locate_arr[0][0],player.locate_arr[1][0]); 
            player.locate_arr[1][0] += 1;
            ShowElem(player);
            
        } 
    }
    else if(game_flag1 == 3) // 向左走
    {
        game_flag1 = 0;
        for(i = 0;i<brick.num;i++)
        {
            if( ((player.locate_arr[1][0])==brick.locate_arr[1][i]) && (player.locate_arr[0][0]-1==brick.locate_arr[0][i]) )
            {
                break;
            }
        }
        for(j = 0;j<box_blue.num;j++)
        {
            if( ((player.locate_arr[1][0])==box_blue.locate_arr[1][j]) && (player.locate_arr[0][0]-1==box_blue.locate_arr[0][j]) )
            {
                game_flag2_box = 3; // 向左走碰到箱子
                updata_box(j,game_flag2_box);
                break;
            }
        }
        if(j == box_blue.num) game_flag3_plug = 1;
        if((i == brick.num) && (game_flag3_plug == 1))
        {
            for(j = 0;j<destination.num;j++)
            {
                if( (player.locate_arr[0][0] == destination.locate_arr[0][j]) && (player.locate_arr[1][0] == destination.locate_arr[1][j]) )
                {
                    ShowPicture(destination.locate_arr[0][j]*20, destination.locate_arr[1][j]*20, 20, 20, destination.picture);
                    break;
                }
            }
            if(j == destination.num) ShowBack(player.locate_arr[0][0],player.locate_arr[1][0]); 
            player.locate_arr[0][0] -= 1;
            ShowElem(player);
            
        } 
    }
    else if(game_flag1 == 4) // 向右走
    {
        game_flag1 = 0;
        for(i = 0;i<brick.num;i++)
        {
            if( ((player.locate_arr[1][0])==brick.locate_arr[1][i]) && (player.locate_arr[0][0]+1==brick.locate_arr[0][i]) )
            {
                break;
            }
        }
        for(j = 0;j<box_blue.num;j++)
        {
            if( ((player.locate_arr[1][0])==box_blue.locate_arr[1][j]) && (player.locate_arr[0][0]+1==box_blue.locate_arr[0][j]) )
            {
                game_flag2_box = 4; // 向右走碰到箱子
                updata_box(j,game_flag2_box);
                break;
            }
        }
        if(j == box_blue.num) game_flag3_plug = 1;  
        if((i == brick.num) && (game_flag3_plug == 1))
        {
            for(j = 0;j<destination.num;j++)
            {
                if( (player.locate_arr[0][0] == destination.locate_arr[0][j]) && (player.locate_arr[1][0] == destination.locate_arr[1][j]) )
                {
                    ShowPicture(destination.locate_arr[0][j]*20, destination.locate_arr[1][j]*20, 20, 20, destination.picture);
                    break;
                }
            }
            if(j == destination.num) ShowBack(player.locate_arr[0][0],player.locate_arr[1][0]); 
            player.locate_arr[0][0] += 1;
            ShowElem(player);
            
        } 
    }
}

void match_box_des()
{
    int amount = 0;
    // 箱子和目的地匹配
    for(i=0;i<box_blue.num;i++)
    {
        for(j = 0;j<destination.num;j++)
        {
            if( (box_blue.locate_arr[0][i]==destination.locate_arr[0][j]) && (box_blue.locate_arr[1][i]==destination.locate_arr[1][j]) )
            {
                ShowPicture(box_blue.locate_arr[0][i]*20,box_blue.locate_arr[1][i]*20,20,20,gImage_box_red_20_20);
                amount++;
            }
        }
    }
    // 胜利判断
    if(amount == box_blue.num)
    {
        game_win = 1;
    }

}

void fresh_lcd()
{
    match_box_des();

    // 更新角色和箱子位置
    update_player();
}

void Lcd_game(void)
{
    // 清屏
    ClearScr(0);

    //按钮绘制
    ShowPicture(20,210,50,50,gImage_up_50_50);
    ShowPicture(90,210,50,50,gImage_down_50_50);
    ShowPicture(160,210,50,50,gImage_left_50_50);
    ShowPicture(230,210,50,50,gImage_right_50_50);
    ShowPicture(300,210,50,50,gImage_restart_50_50);
    ShowPicture(370,210,50,50,gImage_nextlevel_50_50);

    // p为关卡的数目
    for(p = 0;p<2;p++)
    {
        for(i = 0;i<10;i++)
        {
            for(j = 0;j<24;j++)
            ShowPicture(20*j,20*i,20,20,gImage_background_20_20);  // 画背景
        }
        ShowPicture(429,0,50,50,gImage_exit_50_50);                // 画退出标志

        load_data(p+1);                                            // 读取数据
        ShowElem(brick);
        ShowElem(box_blue);
        ShowElem(player);
        ShowElem(destination);

        game_win = 0;
        while(1)
        {
            // 检查重置
            if(game_flag1 == 5)
            {
                game_flag1 = 0;
                load_data(p+1);
                for(i = 0;i<10;i++)
                {
                    for(j = 0;j<24;j++)
                    ShowPicture(20*j,20*i,20,20,gImage_background_20_20);
                }
                ShowPicture(429,0,50,50,gImage_exit_50_50); 
                // 显示元素
                ShowElem(brick);
                ShowElem(box_blue);
                ShowElem(player);
                ShowElem(destination);
            }

            // 更新屏幕
            fresh_lcd();

            // 检查胜利或下一关
            if(game_win == 1 || game_flag1 == 6)
            {
                game_flag1 = 0;
                printf("胜利");
                ShowPicture(150,55,180,90,gImage_next_180_90);
                delay(15);
                break;
            } 

            // 检查退出
            if(state == 0)
            {
                break;
            }
        
        }
        // 检查退出
        if(state == 0)
        {
            break;
        }

    }
    // 游戏结束
    ShowPicture(140,50,200,100,gImage_gameover_200_100);
    delay(30);

}


