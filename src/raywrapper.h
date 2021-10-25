#pragma once

#include "variable_defines.h"

#include "memory.h"

// #define RL_MALLOC(sz)       GET_DATA(sz)
// #define RL_CALLOC(n,sz)     GET_CLEAR_DATA(n,sz)
// #define RL_REALLOC(ptr,sz)  GROW_DATA(ptr,sz)
// #define RL_FREE(ptr)        FREE_DATA(ptr)

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION

enum Editor_Mode_Settings
{
    EDITOR_MODE_GAME = 0,
    EDITOR_MODE_UI,
    EDITOR_MODE_DEBUG,
    EDITOR_MODE_SETTINGS
};

enum Editor_Control_Settings
{
    EDITOR_CONTROL_PLAY = 0,
    EDITOR_CONTROL_EDIT,
    EDITOR_CONTROL_SETTINGS
};

struct Game_State
{
    Editor_Mode_Settings editor_Mode_Settings;
    Editor_Control_Settings editor_Control_Settings;
};

struct System_State
{
    int screen_Width;
    int screen_Height;
    int fps;
    Color clear_Color;
    const char* window_Title;
};

FUNC_SIG void SystemStateSetup(System_State *system_State)
{
    system_State->window_Title = "Game";
    system_State->screen_Width = 1920;
    system_State->screen_Height = 1080;
    system_State->clear_Color = {255, 0, 255, 255}; // hot pink
    system_State->fps = 120;
}

FUNC_SIG void SetupRaylib(System_State *system_State)
{
    InitWindow(system_State->screen_Width, system_State->screen_Height, system_State->window_Title);
    SetTargetFPS(system_State->fps);

    GuiLoadStyle("assets/styles/cyber/cyber.rgs");

    SetExitKey(0);
}

FUNC_SIG void CleanupRaylib()
{
    CloseWindow();
}

enum Debug_Types
{
    CARD_TYPE,
    WINDOW_TYPE,

    VEC2_TYPE,
    VEC3_TYPE,
    RECT_TYPE,
    COLOR_TYPE,

    CHAR_TYPE,
    INT32_TYPE,
    UINT32_TYPE,
    FLOAT32_TYPE,
    BOOL_TYPE,
    STR_TYPE,
};

struct Visual_Card
{
    Rectangle rectangle;

    Vector2 center;
    bool collidable;

    Color color;
    float font_Height;

    smax stack_Value;
    smax card_Index;
    //StandardCard *card;
};

void SwapVisualCards(Visual_Card *card_A, Visual_Card *card_B)
{
    if(card_A == card_B)
        return;

    Visual_Card temp = {};
    temp.rectangle = card_A->rectangle;
    temp.center = card_A->center;
    temp.collidable = card_A->collidable;
    temp.color = card_A->color;
    temp.font_Height = card_A->font_Height;
    temp.stack_Value = card_A->stack_Value;
    temp.card_Index = card_A->card_Index;

    card_A->rectangle = card_B->rectangle;
    card_A->center = card_B->center;
    card_A->collidable = card_B->collidable;
    card_A->color = card_B->color;
    card_A->font_Height = card_B->font_Height;
    card_A->stack_Value = card_B->stack_Value;
    card_A->card_Index = card_B->card_Index;

    card_B->rectangle = temp.rectangle;
    card_B->center = temp.center;
    card_B->collidable = temp.collidable;
    card_B->color = temp.color;
    card_B->font_Height = temp.font_Height;
    card_B->stack_Value = temp.stack_Value;
    card_B->card_Index = temp.card_Index;
}

struct Visual_Board
{
    real32 card_Height;
    real32 card_Width;
    real32 card_Scale;

    Rectangle player_Two_Zone;
    Rectangle play_Zone;
    Rectangle player_One_Zone;
};

void SetupVisualBoard(Visual_Board *visual_Board, real32 width, real32 height)
{
    visual_Board->card_Height = 88.0f;
    visual_Board->card_Width = 63.0f;
    visual_Board->card_Scale = 1.0f;

    real32 half_Width = width / 2.0f;

    visual_Board->player_Two_Zone = {half_Width / 2.0f, 0.0f, half_Width, height / 4.0f};
    visual_Board->play_Zone = {0.0f, visual_Board->player_Two_Zone.height, width, height / 2.0f};
    visual_Board->player_One_Zone = {half_Width / 2.0f, visual_Board->play_Zone.height + visual_Board->player_Two_Zone.height, half_Width, height / 4.0f};

    // Get Card Scale
    // while((visual_Board->card_Width * (visual_Board->card_Scale + 1.0f)) < (visual_Board->play_Zone.width / 6.0f))
    // {
    //     visual_Board->card_Scale += 1.0f;
    // }

    while((visual_Board->card_Height * (visual_Board->card_Scale + 1.0f)) < (visual_Board->play_Zone.height / 2.0f))
    {
        visual_Board->card_Scale += 1.0f;
    }
}

smax GetBoardStep(smax index)
{
    switch(index)
    {
        case 0:
        case 1:
            return 2;
        case 2:
        case 3:
            return 3;
        case 4:
        case 5:
            return 1;
        case 6:
        case 7:
            return 4;
        case 8:
        case 9:
            return 0;
        case 10:
        case 11:
            return 5;
    }
    return 0;
}

void SetupGameBoard(Visual_Board *visual_Board, Visual_Card *game_Cards, smax card_Count)
{
    real32 card_Height = visual_Board->card_Height * visual_Board->card_Scale;
    real32 card_Width = visual_Board->card_Width * visual_Board->card_Scale;
    real32 w_Gap = (visual_Board->play_Zone.width - card_Width * 6) / (6 + 1);
    real32 h_Gap = (visual_Board->play_Zone.height - card_Height * 2) / (2 + 1);

    smax step = 0;

    // even
    for(smax i = 0; i < card_Count; i+=2)
    {
        step = GetBoardStep(i);
        game_Cards[i].rectangle = { visual_Board->play_Zone.x + (real32)(step+1) * w_Gap + (real32)(step) * card_Width, visual_Board->play_Zone.y + h_Gap, card_Width, card_Height };
    }

    // odd
    for(smax i = 1; i < card_Count; i+=2)
    {
        step = GetBoardStep(i);
        game_Cards[i].rectangle = { visual_Board->play_Zone.x + (real32)(step+1) * w_Gap + (real32)(step) * card_Width, visual_Board->play_Zone.y + h_Gap * 2 + card_Height, card_Width, card_Height };
    }
}


void SetupPlayerVisualBoardCard(Visual_Board *visual_Board, Visual_Card *player_Cards, smax card_Count, Rectangle *zone, umax card_Index)
{
    real32 card_Height = visual_Board->card_Height * visual_Board->card_Scale;
    real32 card_Width = visual_Board->card_Width * visual_Board->card_Scale;
    real32 w_Gap = (zone->width - (card_Width * card_Count)) / (card_Count + 1);
    real32 h_Gap = (zone->height - card_Height) / (2);

    player_Cards[card_Index].rectangle = { zone->x + (real32)(card_Index+1) * w_Gap + (real32)(card_Index) * card_Width, zone->y + h_Gap, card_Width, card_Height };
}

void SetupPlayerVisualBoardHand(Visual_Board *visual_Board, Visual_Card *player_Cards, smax card_Count, Rectangle *zone)
{
    real32 card_Height = visual_Board->card_Height * visual_Board->card_Scale;
    real32 card_Width = visual_Board->card_Width * visual_Board->card_Scale;
    real32 w_Gap = (zone->width - (card_Width * card_Count)) / (card_Count + 1);
    real32 h_Gap = (zone->height - card_Height) / (2);

    for(smax i = 0; i < card_Count; ++i)
    {
        player_Cards[i].rectangle = { zone->x + (real32)(i+1) * w_Gap + (real32)(i) * card_Width, zone->y + h_Gap, card_Width, card_Height };
    }
}

void SetupPlayer1Board(Visual_Board *visual_Board, Visual_Card *p1_Cards, smax card_Count)
{
    real32 card_Height = visual_Board->card_Height * visual_Board->card_Scale;
    real32 card_Width = visual_Board->card_Width * visual_Board->card_Scale;
    real32 w_Gap = (visual_Board->player_One_Zone.width - (card_Width * card_Count)) / (card_Count + 1);
    real32 h_Gap = (visual_Board->player_One_Zone.height - card_Height) / (2);

    for(smax i = 0; i < card_Count; ++i)
    {
        p1_Cards[i].rectangle = { visual_Board->player_One_Zone.x + (real32)(i+1) * w_Gap + (real32)(i) * card_Width, visual_Board->player_One_Zone.y + h_Gap, card_Width, card_Height };
    }
}

void SetupPlayer2Board(Visual_Board *visual_Board, Visual_Card *p2_Cards, smax card_Count)
{
    real32 card_Height = visual_Board->card_Height * visual_Board->card_Scale;
    real32 card_Width = visual_Board->card_Width * visual_Board->card_Scale;
    real32 w_Gap = (visual_Board->player_Two_Zone.width - (card_Width * card_Count)) / (card_Count + 1);
    real32 h_Gap = (visual_Board->player_Two_Zone.height - card_Height) / (2);

    for(smax i = 0; i < card_Count; ++i)
    {
        p2_Cards[i].rectangle = { visual_Board->player_Two_Zone.x + (real32)(i+1) * w_Gap + (real32)(i) * card_Width, visual_Board->player_Two_Zone.y + h_Gap, card_Width, card_Height };
    }
}

void UpdateCardCenter(Visual_Card *card)
{
    card->center.x = card->rectangle.x + card->rectangle.width / 2.0f;
    card->center.y = card->rectangle.y + card->rectangle.height / 2.0f;
}

smax ActiveCardBoardCollision(Visual_Card *active_Card, Visual_Card *board_Cards, umax board_Card_Count)
{
    smax index = -1;

    if(!active_Card->collidable)
        return index;

    float dist = 2000.0f;
    float test_Dist = 0.0f;
    for(umax i = 0; i < board_Card_Count; ++i)
    {
        if(active_Card == &(board_Cards[i]) || !board_Cards[i].collidable)
            continue;

        if(CheckCollisionRecs(active_Card->rectangle, board_Cards[i].rectangle))
        {
            test_Dist = Vector2Distance(active_Card->center, board_Cards[i].center);
            if(test_Dist < dist)
            {
                dist = test_Dist;
                index = i;
            }
        }
    }

    return index;
}

struct Debug_Item
{
    const char *name;
    Rectangle *rect;
    Debug_Types type;
    char *entity;

    int gui_Entity_Count;
    char** gui_Entities;
};

struct Debug_Items
{
    int item_Count;
    Debug_Item items[256];
    int item_Hover;
    int item_Selected;
};

struct Gui_Vector2
{
    Gui_Control group;
    Gui_Control labels[3];
};

struct Gui_Window
{
    char* title;
    Vector2 position;
    Vector2 open_Size;
    Rectangle rectangle;
    Vector2 viewScroll;

    bool hover;
    bool clicked;

    bool open;
    bool alive;
    bool drag;
};

FUNC_SIG void InitGuiWIndow(Gui_Window gui_Window)
{
    gui_Window = {0};

    gui_Window.alive = true;
    gui_Window.open = true;
    gui_Window.drag = false;

    gui_Window.hover = false;
    gui_Window.clicked = false;

    gui_Window.title = (char*)"#198# New Window";
    gui_Window.position = { 0.0f, 0.0f };
    gui_Window.open_Size = { 100.0f, 100.0f};
    gui_Window.rectangle = { 0.0f, 0.0f, 100.0f, 100.0f};

    gui_Window.viewScroll = {100.0f, 100.0f};
}

struct Debug_Window
{
    bool alive = false;

    Rectangle window;

    char allTextBox[12][64];
    char *previousTextBox;
    char *activeTextBox;
    int command_Count;

    Rectangle text_Box;
};

#define MAX_DEBUG_LINES 256
struct Debug_Lines
{
    int line_Count;
    Vector2 start_Lines[MAX_DEBUG_LINES];
    Vector2 end_Lines[MAX_DEBUG_LINES];
    Color line_Colors[MAX_DEBUG_LINES];
};
Debug_Lines debug_Lines = {0};

FUNC_SIG int DEBUG_GetLine()
{
    if(debug_Lines.line_Count >= MAX_DEBUG_LINES)
        return -1;

    ++debug_Lines.line_Count;
    return debug_Lines.line_Count - 1;
}

FUNC_SIG void DEBUG_AddLine(int index, Vector2 * start, Vector2 * end, Color color)
{
    if(index > MAX_DEBUG_LINES)
        return;

    debug_Lines.start_Lines[index].x = start->x;
    debug_Lines.start_Lines[index].y = start->y;
    debug_Lines.end_Lines[index].x = end->x;
    debug_Lines.end_Lines[index].y = end->y;
    debug_Lines.line_Colors[index] = color;
}

FUNC_SIG void DEBUG_DrawLines()
{
    if(debug_Lines.line_Count > MAX_DEBUG_LINES)
    {
        debug_Lines.line_Count = MAX_DEBUG_LINES;
    }

    for(int i = 0; i < debug_Lines.line_Count; ++i)
    {
        DrawLineEx(debug_Lines.start_Lines[i], debug_Lines.end_Lines[i], 5, debug_Lines.line_Colors[i]);
    }
}

FUNC_SIG void MoveEntityData(Debug_Item *debug_Item, Vector2 position)
{
    switch(debug_Item->type)
    {
        case CARD_TYPE:
        {
            Visual_Card *temp_Card = (Visual_Card*)debug_Item->entity;
            temp_Card->rectangle.x = position.x;
            temp_Card->rectangle.y = position.y;
        }break;
    }
}

//struct Visual_Card
//{
//
//    Color color;
//    float font_Height;
//
//    smax stack_Value;
//    smax card_Index;
//};

FUNC_SIG void SetupDebugGuiData(Debug_Item *debug_Item)
{ 
    switch(debug_Item->type)
    {
        case CARD_TYPE:
        {
            Visual_Card *temp_Card = (Visual_Card*)debug_Item->entity;

            Gui_EditValueFloatControl *rectX = 0;
            STORE_PERM_DATA(rectX, sizeof(Gui_EditValueFloatControl));
            SetupGuiValueBox(rectX);
            rectX->value = &temp_Card->rectangle.x;
            rectX->text = "X";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectY = 0;
            STORE_PERM_DATA(rectY, sizeof(Gui_EditValueFloatControl));
            SetupGuiValueBox(rectY);
            rectY->value = &temp_Card->rectangle.y;
            rectY->text = "Y";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectW = 0;
            STORE_PERM_DATA(rectW, sizeof(Gui_EditValueFloatControl));
            SetupGuiValueBox(rectW);
            rectW->value = &temp_Card->rectangle.width;
            rectW->text = "W";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectH = 0;
            STORE_PERM_DATA(rectH, sizeof(Gui_EditValueFloatControl));
            SetupGuiValueBox(rectH);
            rectH->value = &temp_Card->rectangle.height;
            rectH->text = "H";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *center_X = 0;
            STORE_PERM_DATA(center_X, sizeof(Gui_EditValueFloatControl));
            SetupGuiValueBox(center_X);
            center_X->value = &temp_Card->center.x;
            center_X->text = "Center X";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *center_Y = 0;
            STORE_PERM_DATA(center_Y, sizeof(Gui_EditValueFloatControl));
            SetupGuiValueBox(center_Y);
            center_Y->value = &temp_Card->center.y;
            center_Y->text = "Center Y";
            ++debug_Item->gui_Entity_Count;

            Gui_EditActiveControl *collidable = 0;
            STORE_PERM_DATA(collidable, sizeof(Gui_EditActiveControl));
            collidable->bounds = {0};
            collidable->text = "Collidable";
            collidable->active = &temp_Card->collidable;
            collidable->edit_Mode = false;
            ++debug_Item->gui_Entity_Count;

            debug_Item->gui_Entities = 0;
            STORE_PERM_DATA(debug_Item->gui_Entities, sizeof(Gui_EditValueFloatControl*) * debug_Item->gui_Entity_Count);

            debug_Item->gui_Entities[0] = (char*)rectX;
            debug_Item->gui_Entities[1] = (char*)rectY;
            debug_Item->gui_Entities[2] = (char*)rectW;
            debug_Item->gui_Entities[3] = (char*)rectH;

            debug_Item->gui_Entities[4] = (char*)center_X;
            debug_Item->gui_Entities[5] = (char*)center_Y;

            debug_Item->gui_Entities[6] = (char*)collidable;
        }break;
        case WINDOW_TYPE:
        {
            Gui_Window *temp_Window = (Gui_Window*)debug_Item->entity;

            Gui_EditValueFloatControl *posX = 0; //(Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            STORE_PERM_DATA(posX, sizeof(Gui_EditValueFloatControl));
            SetupGuiValueBox(posX);
            posX->value = &temp_Window->position.x;
            posX->text = "X";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *posY = 0; //(Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            STORE_PERM_DATA(posY, sizeof(Gui_EditValueFloatControl));
            SetupGuiValueBox(posY);
            posY->value = &temp_Window->position.y;
            posY->text = "Y";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectX = 0; //(Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            STORE_PERM_DATA(rectX, sizeof(Gui_EditValueFloatControl));
            SetupGuiValueBox(rectX);
            rectX->value = &temp_Window->rectangle.x;
            rectX->text = "X";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectY = 0; //(Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            STORE_PERM_DATA(rectY, sizeof(Gui_EditValueFloatControl));
            SetupGuiValueBox(rectY);
            rectY->value = &temp_Window->rectangle.y;
            rectY->text = "Y";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectW = 0; //(Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            STORE_PERM_DATA(rectW, sizeof(Gui_EditValueFloatControl));
            SetupGuiValueBox(rectW);
            rectW->value = &temp_Window->rectangle.width;
            rectW->text = "W";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectH = 0; //(Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            STORE_PERM_DATA(rectH, sizeof(Gui_EditValueFloatControl));
            SetupGuiValueBox(rectH);
            rectH->value = &temp_Window->rectangle.height;
            rectH->text = "H";
            ++debug_Item->gui_Entity_Count;

            //debug_Item->gui_Entities = (char**)malloc(sizeof(Gui_EditValueFloatControl*) * debug_Item->gui_Entity_Count);
            debug_Item->gui_Entities = 0;
            STORE_PERM_DATA(debug_Item->gui_Entities, sizeof(Gui_EditValueFloatControl*) * debug_Item->gui_Entity_Count);

            debug_Item->gui_Entities[0] = (char*)posX;
            debug_Item->gui_Entities[1] = (char*)posY;
            debug_Item->gui_Entities[2] = (char*)rectX;
            debug_Item->gui_Entities[3] = (char*)rectY;
            debug_Item->gui_Entities[4] = (char*)rectW;
            debug_Item->gui_Entities[5] = (char*)rectH;
        }break;
    }
}

FUNC_SIG void MouseTests()
{

}

//struct Visual_Card
//{
//
//    Color color;
//    float font_Height;
//
//    smax stack_Value;
//    smax card_Index;
//};
FUNC_SIG void DrawType(Rectangle window_Slice, Debug_Types type, char** gui_Entities, int gui_Entity_Count)
{
    float x_Edge = 5.0f;
    float y_Edge = 5.0f;

    float text_Height = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textBox_Padding = GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)  + GuiGetStyle(TEXTBOX, TEXT_LINES_PADDING);
    float total = text_Height + textBox_Padding + 10.0f;

    float vec_Width = (window_Slice.width - x_Edge * 3.0f - textBox_Padding * 2.0f) / 2.0f;
    float vec_Height = (window_Slice.height - y_Edge * 3.0f) / 2.0f;

    switch(type)
    {
        case CARD_TYPE:
        {
            Rectangle rect_Rect = {window_Slice.x + x_Edge, 
                                   window_Slice.y + y_Edge, 
                                   window_Slice.width - 2.0f * x_Edge, 
                                   total * 2.0f};
            GuiGroupBox(rect_Rect, "Rect");
            DrawType(rect_Rect, RECT_TYPE, gui_Entities, 4);

            Rectangle center_Rect = {window_Slice.x + x_Edge, 
                                   window_Slice.y + rect_Rect.height + y_Edge * 2.0f, 
                                   window_Slice.width - 2.0f * x_Edge, 
                                   total};
            GuiGroupBox(center_Rect, "Center");
            DrawType(center_Rect, VEC2_TYPE, &gui_Entities[4], 2);

            Rectangle collidable_Rect = {window_Slice.x + x_Edge, 
                                   window_Slice.y + rect_Rect.height + center_Rect.height + y_Edge * 3.0f, 
                                   window_Slice.width - 2.0f * x_Edge, 
                                   total};
            GuiGroupBox(collidable_Rect, "Collidable");
            DrawType(collidable_Rect, BOOL_TYPE, &gui_Entities[6], 1);


        }break;
        case WINDOW_TYPE:
        {
            Rectangle position_Rect = {window_Slice.x + x_Edge, 
                                       window_Slice.y + y_Edge, 
                                       window_Slice.width - 2.0f * x_Edge, 
                                       total};
            GuiGroupBox(position_Rect, "Position");
            DrawType(position_Rect, VEC2_TYPE, gui_Entities, 2);

            Rectangle rect_Rect = {window_Slice.x + x_Edge, 
                                   window_Slice.y + position_Rect.height + y_Edge * 2.0f, 
                                   window_Slice.width - 2.0f * x_Edge, 
                                   total * 2.0f};
            GuiGroupBox(rect_Rect, "Rect");
            DrawType(rect_Rect, RECT_TYPE, &gui_Entities[2], 4);
        }break;
        case BOOL_TYPE:
        {
            Gui_EditActiveControl *temp_boolToggleControl = (Gui_EditActiveControl*)(gui_Entities[0]);

            temp_boolToggleControl->bounds =
            {
                window_Slice.x + text_Height * 5.0f + textBox_Padding, 
                window_Slice.y, 
                15.0f , 
                window_Slice.height 
            };

            *(temp_boolToggleControl->active) = GuiCheckBox(temp_boolToggleControl);

            DrawRectangleLinesEx(window_Slice, 1, RED);
            DrawRectangleLinesEx(temp_boolToggleControl->bounds, 1, RED);
        }break;
        case FLOAT32_TYPE:
        {
            Gui_EditValueFloatControl *temp_FloatControl = (Gui_EditValueFloatControl*)(gui_Entities[0]);

            temp_FloatControl->bounds =
            {
                window_Slice.x + text_Height * 5.0f + textBox_Padding, 
                window_Slice.y, 
                window_Slice.width - textBox_Padding - text_Height * 5.0f , 
                window_Slice.height 
            };

            if(GuiValueBox(temp_FloatControl))
            {
                temp_FloatControl->edit_Mode = !temp_FloatControl->edit_Mode;
            }
            DrawRectangleLinesEx(window_Slice, 1, RED);
            DrawRectangleLinesEx(temp_FloatControl->bounds, 1, RED);
        }break;
        case RECT_TYPE:
        {
            Gui_EditValueFloatControl *temp_RectX = (Gui_EditValueFloatControl*)(gui_Entities[0]);
            Gui_EditValueFloatControl *temp_RectY = (Gui_EditValueFloatControl*)(gui_Entities[1]);
            Gui_EditValueFloatControl *temp_RectW = (Gui_EditValueFloatControl*)(gui_Entities[2]);
            Gui_EditValueFloatControl *temp_RectH = (Gui_EditValueFloatControl*)(gui_Entities[3]);

            temp_RectX->bounds = {window_Slice.x + x_Edge + textBox_Padding, 
                                  window_Slice.y + y_Edge, 
                                  vec_Width, 
                                  vec_Height};
            if(GuiValueBox(temp_RectX))
            {
                temp_RectX->edit_Mode = !temp_RectX->edit_Mode;
            }

            temp_RectY->bounds = temp_RectX->bounds;
            temp_RectY->bounds.x += x_Edge + textBox_Padding + vec_Width;
            if(GuiValueBox(temp_RectY))
            {
                temp_RectY->edit_Mode = !temp_RectY->edit_Mode;
            }

            temp_RectW->bounds = temp_RectX->bounds;
            temp_RectW->bounds.y += y_Edge + vec_Height;
            if(GuiValueBox(temp_RectW))
            {
                temp_RectW->edit_Mode = !temp_RectW->edit_Mode;
            }

            temp_RectH->bounds = temp_RectX->bounds;
            temp_RectH->bounds.x += x_Edge + textBox_Padding + vec_Width;
            temp_RectH->bounds.y += y_Edge + vec_Height;
            if(GuiValueBox(temp_RectH))
            {
                temp_RectH->edit_Mode = !temp_RectH->edit_Mode;
            }
        }break;
        case VEC2_TYPE:
        {
            Gui_EditValueFloatControl *temp_VecX = (Gui_EditValueFloatControl*)(gui_Entities[0]);
            Gui_EditValueFloatControl *temp_VecY = (Gui_EditValueFloatControl*)(gui_Entities[1]);

            temp_VecX->bounds = {window_Slice.x + x_Edge + textBox_Padding, 
                               window_Slice.y + y_Edge, 
                               vec_Width, 
                               window_Slice.height - y_Edge * 2.0f};
            if(GuiValueBox(temp_VecX))
            {
                temp_VecX->edit_Mode = !temp_VecX->edit_Mode;
            }

            temp_VecY->bounds = temp_VecX->bounds;
            temp_VecY->bounds.x += x_Edge + textBox_Padding + vec_Width;
            if(GuiValueBox(temp_VecY))
            {
                temp_VecY->edit_Mode = !temp_VecY->edit_Mode;
            }
        }break;
    }
}

FUNC_SIG void DebugEditor(Gui_Window *editor_Window, Debug_Item * debug_Item)
{
    float text_Height = GuiGetStyle(DEFAULT, TEXT_SIZE) / 2.0f;
    int statusBarHeight = WINDOW_STATUSBAR_HEIGHT + 2;
    int border_Width = 2 * GuiGetStyle(STATUSBAR, BORDER_WIDTH);
    int slider_Size = GuiGetStyle(SLIDER, SLIDER_WIDTH);

    float x_Edge = 5.0f;
    float y_Edge = 15.0f;

    Rectangle view = GuiScrollPanel( 
        { editor_Window->rectangle.x + x_Edge, 
        editor_Window->rectangle.y + WINDOW_STATUSBAR_HEIGHT + 2, 
        editor_Window->rectangle.width - 10.0f, 
        editor_Window->rectangle.height - 7.0f - WINDOW_STATUSBAR_HEIGHT}, 
        
        { editor_Window->rectangle.x + 5.0f, 
        editor_Window->rectangle.y + WINDOW_STATUSBAR_HEIGHT + 2, 
        (float)GetScreenWidth(), 
        (float)GetScreenHeight() }, &editor_Window->viewScroll);

    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);

    Rectangle window_Slice = 
    {
        editor_Window->rectangle.x + x_Edge,
        editor_Window->rectangle.y + statusBarHeight + y_Edge,
        editor_Window->rectangle.width - slider_Size - 5.0f,
        editor_Window->rectangle.height - slider_Size - statusBarHeight - y_Edge
    };

    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    GuiLabel({window_Slice.x, window_Slice.y, window_Slice.width, 0.0f}, debug_Item->name);

    window_Slice.y += text_Height;
    window_Slice.height -= text_Height;
    DrawType(window_Slice, debug_Item->type, debug_Item->gui_Entities, debug_Item->gui_Entity_Count);

    EndScissorMode();
}

FUNC_SIG void UpdateCameraPlayerBoundsPush(Camera2D *camera, Visual_Card *cards, int card_Count, float delta, int width, int height) 
{
    static Vector2 bbox = { 0.2f, 0.2f };

    Vector2 bboxWorldMin = GetScreenToWorld2D({ (1.0f - bbox.x) * 0.5f * width, (1.0f - bbox.y) * 0.5f * height }, *camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D({ (1.0f + bbox.x) * 0.5f * width, (1.0f + bbox.y) * 0.5f * height }, *camera);
    camera->offset = { (1.0f - bbox.x) * 0.5f * width, (1 - bbox.y)*0.5f*height };

    Vector2 edge = camera->target;

    // if(player->position.x < bboxWorldMin.x)
    // {
    //     edge.x = player->position.x;
    // }
    // else if(player->postion.x > bboxWorldMax.x)
    // {
    //     edge.x = bboxWorldMin.x + (player->position.x - bboxWorldMax.x);
    // }

    // if(player->position.y < bboxWorldMin.y)
    // {
    //     edge.y = player->position.y;
    // }    else if(player->position.y > bboxWorldMax.y)
    // {
    //     edge.y = bboxWorldMin.y + (player->position.y - bboxWorldMax.y);
    // }

    static float minSpeed = 60.0f;
    static float minEffectLength = 1.0f;
    static float fractionSpeed = 0.8f;

    Vector2 diff = Vector2Subtract(edge, camera->target);
    float length = Vector2Length(diff);
    
    if(length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed*length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed*delta/length));
    }
 }
