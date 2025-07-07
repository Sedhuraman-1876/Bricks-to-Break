#include <raylib.h>
#include <iostream>
#define sw 500
#define sh 600
#define st "Bricks To Break"
int framecount = 0;
int score = 0, target = 0;
enum state
{
  start,
  gameplay,
  exitreq,
  none,
  pause,
  end,
  controls
};
enum brickspattern
{
  plain = 0,
  mix,
  randomise
};
bool evod(int i)
{
  if (i % 2 == 0)
    return true;
  return false;
}
class Player
{
public:
  Rectangle playerbody = {0, 0, 60, 20};
  Texture2D playertexture;
  Vector2 playerposition;
  int playerspeed = 5;
  Player()
  {
    playertexture = LoadTexture("pic/paddleBlu.png");
    playertexture.width = 60;
    playertexture.height = 20;
    playerposition = {220, 450};
  }
  ~Player()
  {
    UnloadTexture(playertexture);
  }
  void DrawPlayer()
  {
    DrawTextureRec(playertexture, playerbody, playerposition, WHITE);
  }
  void PlayerMovement()
  {
    if (IsKeyDown(KEY_A))
    {
      if (playerposition.x > 0)
        playerposition.x -= playerspeed;
    }
    if (IsKeyDown(KEY_S))
    {
      if (playerposition.x < (sw - playerbody.width))
        playerposition.x += playerspeed;
    }
  }
};
class Ball : public Player
{
public:
  Rectangle ballbody = {0, 0, 20, 20};
  Texture2D balltexture;
  Vector2 ballposition;
  Vector2 ballspeed = {3, -3};
  Rectangle border = {0, 470, sw, 20};
  bool movement = false;
  bool servestate = true;
  Ball()
  {
    balltexture = LoadTexture("pic/ballBlue.png");
    balltexture.width = ballbody.width;
    balltexture.height = ballbody.height;
    ballposition.y = (playerposition.y - ballbody.height) - 5;
    ballposition.x = (playerposition.x + playerbody.width / 2) - (ballbody.width / 2);
  }
  ~Ball()
  {
    UnloadTexture(balltexture);
  }
  void DrawBall()
  {
    DrawTextureRec(balltexture, ballbody, ballposition, WHITE);
    DrawRectangleRec(border, Fade(WHITE, 0.5));
    if (servestate)
    {
      if ((framecount / 30) % 2 == 0)
      {
        DrawRectangle(0, 240, sw, 60, WHITE);
        DrawText("Press SPACE to START", 60, 255, 30, VIOLET);
      }
    }
  }
  void BallMovement(Player &p)
  {
    if (!movement)
    {
      ballposition.y = (p.playerposition.y - ballbody.height) - 5;
      ballposition.x = (p.playerposition.x + p.playerbody.width / 2) - (ballbody.width / 2);
    }
    if (IsKeyPressed(KEY_SPACE) && !movement)
    {
      movement = true;
      servestate = false;
      ballspeed.x = 3 ? 0 == GetRandomValue(0, 1) : -3;
    }
    if (movement)
    {
      ballposition.y += ballspeed.y;
      if (ballposition.y < 0)
        ballspeed.y = 3;
      if (CheckCollisionRecs(Rectangle{p.playerposition.x, p.playerposition.y, p.playerbody.width, p.playerbody.height}, Rectangle{ballposition.x, ballposition.y, ballbody.width, ballbody.height}))
      {
        switch (GetRandomValue(0, 3))
        {
        case 0:
        {
          ballspeed.y = -3;
          break;
        }
        case 1:
        {
          ballspeed = {3, -3};
          break;
        }
        case 2:
        {
          ballspeed = {-3, -3};
          break;
        }
        case 3:
        {
          ballspeed = {3.5, -3.5};
          break;
        }
        case 4:
        {
          ballspeed = {-3.5, -3.5};
        }
        }
      }
      ballposition.x += ballspeed.x;
      if (ballposition.x > (sw - ballbody.width))
        ballspeed.x = -3;
      if (ballposition.x < 0)
        ballspeed.x = 3;
    }
  }
};
class Bricks
{
public:
  brickspattern cur_pattern, prev_pattern;
  Texture2D rect_texture[6], sqr_texture;
  Rectangle rect = {0, 0, 60, 30}, sqr = {0, 0, 30, 30};
  int *texture_no;
  Vector2 *bricksposition;
  bool *collision;
  int x, y, k = 0, no_of_bricks;
  void setup_Brick()
  {
    x = 3;
    y = 10;
    no_of_bricks = 0;
    target = 0;
    switch (GetRandomValue(0, 2))
    {
    case 0:
    {
      cur_pattern = plain;
      k = 0;
      no_of_bricks = 24;
      bricksposition = new Vector2[no_of_bricks];
      collision = new bool[no_of_bricks];
      *collision = {false};
      rect_texture[k] = LoadTexture("pic/element_blue_rectangle.png");
      rect_texture[k].width = rect.width;
      rect_texture[k].height = rect.height;
      for (int i = 0; i < no_of_bricks; i++)
      {
        target += rect.width;
        bricksposition[i].x = x;
        bricksposition[i].y = y;
        x += (rect.width + 2);
        if (x > 460)
        {
          y += (rect.height + 2);
          x = 3;
        }
      }
      break;
    }
    case 1:
    {
      cur_pattern = mix;
      k = 1;
      x = 10;
      no_of_bricks = 30;
      bricksposition = new Vector2[no_of_bricks];
      collision = new bool[no_of_bricks];
      *collision = {false};
      rect_texture[k] = LoadTexture("pic/element_red_rectangle.png");
      rect_texture[k].width = rect.width;
      rect_texture[k].height = rect.height;
      sqr_texture = LoadTexture("pic/element_grey_square.png");
      sqr_texture.width = sqr.width;
      sqr_texture.height = sqr.height;
      for (int i = 0; i < no_of_bricks; i++)
      {
        bricksposition[i].x = x;
        bricksposition[i].y = y;
        if (evod(i) || i == 0)
        {
          x += (rect.width + 3);
          target += rect.width;
        }
        else
        {
          x += (sqr.width + 3);
          target += sqr.width;
        }
        if (x > 460)
        {
          y += rect.height;
          x = 10;
        }
      }
      break;
    }
    case 2:
    {
      cur_pattern = randomise;
      no_of_bricks = 24;
      bricksposition = new Vector2[no_of_bricks];
      collision = new bool[no_of_bricks];
      *collision = {false};
      texture_no = new int[no_of_bricks];
      rect_texture[k] = LoadTexture("pic/element_blue_rectangle.png");
      rect_texture[k + 1] = LoadTexture("pic/element_green_rectangle.png");
      rect_texture[k + 2] = LoadTexture("pic/element_grey_rectangle.png");
      rect_texture[k + 3] = LoadTexture("pic/element_purple_rectangle.png");
      rect_texture[k + 4] = LoadTexture("pic/element_red_rectangle.png");
      rect_texture[k + 5] = LoadTexture("pic/element_yellow_rectangle.png");
      for (int i = 0; i < 6; i++)
      {
        rect_texture[k + i].width = rect.width;
        rect_texture[k + i].height = rect.height;
      }
      for (int i = 0; i < no_of_bricks; i++)
      {
        target += rect.width;
        bricksposition[i].x = x;
        bricksposition[i].y = y;
        x += (rect.width + 2);
        if (x > 460)
        {
          y += (rect.height + 2);
          x = 3;
        }
        texture_no[i] = GetRandomValue(0, 5);
      }
    }
    }
  }
  ~Bricks()
  {
    UnloadTexture(*rect_texture);
    UnloadTexture(sqr_texture);
    delete collision;
    delete bricksposition;
    delete texture_no;
  }
  void DrawBricks(Ball &b)
  {
    for (int i = 0; i < no_of_bricks; i++)
    {
      if (CheckCollisionRecs(Rectangle{bricksposition[i].x, bricksposition[i].y, rect.width, rect.height}, Rectangle{b.ballposition.x, b.ballposition.y, b.ballbody.width, b.ballbody.height}) && !collision[i])
      {
        if (cur_pattern == plain || cur_pattern == randomise)
          score += rect.width;
        if (cur_pattern == mix)
        {
          if (evod(i))
            score += rect.width;
          else
            score += sqr.width;
        }
        collision[i] = true;
        switch (GetRandomValue(0, 1))
        {
        case 0:
        {
          b.ballspeed = {-3, 3};
          break;
        }
        case 1:
        {
          b.ballspeed = {3, 3};
          break;
        }
        }
      }
      if (!collision[i])
      {
        switch (cur_pattern)
        {
        case plain:
        {
          DrawTextureRec(rect_texture[k], rect, bricksposition[i], WHITE);
          break;
        }
        case mix:
        {
          if (evod(i) || i == 0)
            DrawTextureRec(rect_texture[k], rect, bricksposition[i], WHITE);
          else
            DrawTextureRec(sqr_texture, sqr, bricksposition[i], WHITE);
          break;
        }
        case randomise:
        {
          DrawTextureRec(rect_texture[texture_no[i]], rect, bricksposition[i], WHITE);
          break;
        }
        }
      }
    }
  }
};
class Heart : public Ball
{
public:
  Texture2D texture_heart;
  Rectangle dest_rect = {0, 0, 90, 30};
  Vector2 heart_posisiton = {20, 530};
  int no_of_heart = 3, x = 20, y = 530;
  Heart()
  {
    texture_heart = LoadTexture("pic/life.png");
    texture_heart.height = dest_rect.height;
    texture_heart.width = 90;
  }
  ~Heart()
  {
    UnloadTexture(texture_heart);
  }
  void UpdateHeart(Ball &b)
  {
    if (CheckCollisionRecs(b.border, Rectangle{b.ballposition.x, b.ballposition.y, b.ballbody.width, b.ballbody.height}))
    {
      b.movement = false;
      b.servestate = true;
      no_of_heart--;
      dest_rect.width -= 30;
    }
  }
  void DrawHeart()
  {
    DrawTextureRec(texture_heart, dest_rect, heart_posisiton, WHITE);
    DrawRectangleRounded(Rectangle{200, 530, 100, 30}, 0.5, 1, WHITE);
    DrawRectangleRounded(Rectangle{370, 530, 100, 30}, 0.5, 1, RED);
    DrawText("Score", 210, 500, 25, WHITE);
    DrawText("Target", 370, 500, 25, WHITE);
    DrawText(TextFormat("%i", score), 210, 535, 23, VIOLET);
    DrawText(TextFormat("%i", target), 380, 535, 23, WHITE);
  }
};
void reposition(Player &p, Ball &b, Bricks &r, Heart &h)
{
  p.playerposition = {220, 450};
  b.movement = false;
  b.servestate = true;
  h.no_of_heart = 3;
  h.dest_rect = {0, 0, 90, 30};
  score = 0;
  framecount = 0;
  for (int i = 0; i < r.no_of_bricks; i++)
  {
    r.collision[i] = false;
  }
}
int main()
{
  InitWindow(sw, sh, st);
  SetTargetFPS(60);
  state curstate = start;
  state instate;
  bool exit = false, update = true;
  Vector2 Mouse;
  Player player = Player();
  Ball ball = Ball();
  Bricks brick;
  Heart heart = Heart();
  std::string status;
  while (!exit)
  {
    // updation
    if (curstate == start)
    {
      if (IsKeyPressed(KEY_P) && instate != controls && instate != exitreq)
      {
        brick.setup_Brick();
        curstate = gameplay;
      }
      if (IsKeyPressed(KEY_E) && instate != controls)
        instate = exitreq;
      if (instate == exitreq)
      {
        if (IsKeyPressed(KEY_Y))
          exit = true;
        if (IsKeyPressed(KEY_N))
          instate = none;
      }
      if (IsKeyPressed(KEY_C) && instate != exitreq)
        instate = controls;
      if (instate == controls)
      {
        if (IsKeyPressed(KEY_B))
          instate = none;
      }
    }
    if (curstate == gameplay)
    {
      framecount++;
      if (heart.no_of_heart == 0 || score == target)
      {
        if (heart.no_of_heart == 0)
          status = "Game-Over";
        else
          status = "Win";
        instate = end;
      }
      if (instate == end)
      {
        if (IsKeyPressed(KEY_Q))
        {
          instate = none;
          curstate = start;
          reposition(player, ball, brick, heart);
        }
        if (IsKeyPressed(KEY_R))
        {
          reposition(player, ball, brick, heart);
          instate = none;
        }
      }
      if (instate != pause && instate != end)
      {
        player.PlayerMovement();
        ball.BallMovement(player);
        heart.UpdateHeart(ball);
      }
      if (IsKeyPressed(KEY_ESCAPE) && instate != end)
        instate = pause;
      if (instate == pause)
      {
        if (IsKeyPressed(KEY_C))
          instate = none;
        if (IsKeyPressed(KEY_R))
        {
          reposition(player, ball, brick, heart);
          instate = none;
        }
        if (IsKeyPressed(KEY_Q))
        {
          instate = none;
          curstate = start;
          reposition(player, ball, brick, heart);
          brick.setup_Brick();
        }
      }
    }
    // drawing
    BeginDrawing();
    ClearBackground(BLACK);
    if (curstate == start)
    {
      DrawRectangle(0, 0, sw, sh, Fade(VIOLET, 0.5));
      DrawRectangle(0, 20, sw / 2, 50, VIOLET);
      DrawText("BRICKS", 30, 25, 50, WHITE);
      DrawRectangle(220, 70, 300, 50, WHITE);
      DrawText("TO BREAK", 230, 75, 50, VIOLET);
      DrawRectangleRounded(Rectangle{160, 250, 150, 50}, 1, 1, VIOLET);
      DrawRectangleRoundedLinesEx(Rectangle{160, 250, 150, 50}, 1, 1, 5, WHITE);
      DrawRectangleRounded(Rectangle{160, 320, 150, 50}, 1, 1, VIOLET);
      DrawRectangleRoundedLinesEx(Rectangle{160, 320, 150, 50}, 1, 1, 5, WHITE);
      DrawRectangleRounded(Rectangle{160, 390, 150, 50}, 1, 1, VIOLET);
      DrawRectangleRoundedLinesEx(Rectangle{160, 390, 150, 50}, 1, 1, 5, WHITE);
      DrawText("[P]Play", 190, 265, 25, WHITE);
      DrawText("[C]Controls", 180, 335, 20, WHITE);
      DrawText("[E]Exit", 200, 403, 25, WHITE);
      if (instate == controls)
      {
        DrawRectangle(0, 0, sw, sh, Fade(WHITE, 0.7));
        DrawRectangle(0, 200, sw, 210, VIOLET);
        DrawRectangle(0, 220, 370, 40, GRAY);
        DrawRectangle(130, 280, 370, 40, GRAY);
        DrawRectangle(0, 340, 300, 40, GRAY);
        DrawText("[->]Right arrow to move right", 20, 230, 24, WHITE);
        DrawText("[<-]Left arrow to move left", 150, 290, 24, WHITE);
        DrawText("[ESC]Escape to pause", 20, 350, 24, WHITE);
        DrawRectangle(400, 340, 100, 40, WHITE);
        DrawText("[B]Back", 410, 350, 24, GRAY);
      }
      if (instate == exitreq)
      {
        DrawRectangle(0, 0, sw, sh, Fade(WHITE, 0.3));
        DrawRectangle(0, 200, sw, 200, GRAY);
        DrawRectangle(20, 230, 400, 50, VIOLET);
        DrawText("Do You want to Exit...?", 50, 245, 30, WHITE);
        DrawRectangle(240, 320, 100, 40, RED);
        DrawText("[Y]Yes", 250, 330, 25, WHITE);
        DrawRectangle(370, 320, 100, 40, WHITE);
        DrawText("[N]No", 390, 330, 25, VIOLET);
      }
    }
    if (curstate == gameplay)
    {
      DrawRectangle(0, 0, sw, sh, Fade(VIOLET, 0.5));
      player.DrawPlayer();
      ball.DrawBall();
      brick.DrawBricks(ball);
      heart.DrawHeart();
      if (instate == pause)
      {
        DrawRectangle(0, 0, sw, sh, Fade(WHITE, 0.5));
        DrawRectangleRounded(Rectangle{100, 200, 300, 200}, 0.5, 1, VIOLET);
        DrawRectangleRoundedLinesEx(Rectangle{100, 200, 300, 200}, 0.5, 1, 10, WHITE);
        DrawRectangleRounded(Rectangle{175, 230, 150, 40}, 1, 1, WHITE);
        DrawRectangleRounded(Rectangle{175, 280, 150, 40}, 1, 1, GRAY);
        DrawRectangleRounded(Rectangle{175, 330, 150, 40}, 1, 1, RED);
        DrawText("[C]Continue", 180, 240, 25, VIOLET);
        DrawText("[R]Retry", 200, 290, 25, WHITE);
        DrawText("[Q]Quit", 215, 340, 25, WHITE);
      }
      if (instate == end)
      {
        DrawRectangle(0, 0, sw, sh, Fade(WHITE, 0.3));
        DrawRectangle(0, 200, sw, 200, GRAY);
        DrawRectangle(175, 220, 150, 50, WHITE);
        DrawRectangleRounded(Rectangle{100, 300, 100, 30}, 0.5, 1, VIOLET);
        DrawText("Score", 20, 300, 25, WHITE);
        DrawText(TextFormat("%i", score), 110, 305, 24, WHITE);
        DrawRectangleRounded(Rectangle{380, 300, 100, 30}, 0.5, 1, RED);
        DrawText("Target", 285, 300, 25, WHITE);
        DrawText(TextFormat("%i", target), 390, 305, 24, WHITE);
        DrawRectangle(125, 350, 110, 40, VIOLET);
        DrawRectangle(270, 350, 100, 40, RED);
        DrawText("[R]Retry", 133, 360, 23, WHITE);
        DrawText("[Q]Quit", 285, 360, 23, WHITE);
        if (status == "Win")
          DrawText("Win", 230, 230, 30, VIOLET);
        else
          DrawText("Game Over", 185, 233, 27, RED);
      }
    }
    EndDrawing();
  }
  return 0;
}