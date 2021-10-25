#include <raylib.h>
#include <cmath>
#include <cstdio>


// Turn off visual studio compiler warnings
#pragma warning(disable : 4201) // we want to allow anonymous unions and structs
#pragma warning(disable : 4514) // stdlib abs/div being unreferenced
#pragma warning(disable : 4668) // #if not_defined treated as #if 0
#pragma warning(disable : 4710) // the compiler doesnt inline printf
#pragma warning(disable : 4820) // ignore byte padding issues
#pragma warning(disable : 4996) // freopen is plenty safe for us
#pragma warning(disable : 5039) // we dont care if callbacks throw exceptions
#pragma warning(disable : 5045) // we aren't worried about spectre mitigation

// system setup and defines
#include "variable_defines.h"
#include "system_defines.h"

// ray lib stuff
#include "raywrapper.h"
#include "temp_gui.h"

// other
#include "memory.h"
#include "char_array.h"

// card game code
#include "casina_game.h"

void DrawVisualCards(Visual_Card *visual_Cards, StandardCardSet *card_Set, umax card_Count)
{
    for(umax i = 0; i < card_Count; i++)
    {
        if(visual_Cards[i].card_Index < 0)
        {
            continue;
        }

        DrawRectangleRec(visual_Cards[i].rectangle, visual_Cards[i].color);
        Rectangle card_Value_Rect = {visual_Cards[i].rectangle.x, visual_Cards[i].rectangle.y + visual_Cards[i].rectangle.height / 2.0f, visual_Cards[i].rectangle.width + 50.0f, visual_Cards[i].rectangle.height};
        const char * suit_String = GetSuitString(card_Set->cards[visual_Cards[i].card_Index]->suit);
        const char * value_String = GetValueString(card_Set->cards[visual_Cards[i].card_Index]->value);
        char * final_ID = 0;
        umax final_ID_Length = TextLength(suit_String) + TextLength(value_String) + 1;
        STORE_TEMP_DATA(final_ID, final_ID_Length);
        int length = 0;
        TextCopy(final_ID + length, value_String);
        length += TextLength(value_String);
        TextCopy(final_ID + length, " ");
        ++length;
        TextCopy(final_ID + length, suit_String);
        length += TextLength(suit_String);
        TextCopy(final_ID + length, " ");
        ++length;
        itoa(visual_Cards[i].stack_Value, final_ID + length, 10);
        DrawTextRec(GetFontDefault(), final_ID, card_Value_Rect, 30.0f, 2.0f, false, LIGHTGRAY);
    }
}

void UpdateVisualCards(Visual_Card *visual_Cards, smax starting_Card, smax card_Count)
{
    for(smax i = starting_Card; i < card_Count; ++i)
    {
        --visual_Cards[i].card_Index;
    }
}

void SetupVisualCards(CasinaBoard *casina_Board, Visual_Board *visual_Board, Visual_Card *p1_Cards, Visual_Card *p2_Cards)
{
    smax p2_Cards_Count = 4;
    SetupPlayerVisualBoardHand(visual_Board, p2_Cards, p2_Cards_Count, &visual_Board->player_Two_Zone);
    for(umax i = 0; i < casina_Board->player2.hand.card_Count; ++i)
    {
        p2_Cards[i].center = {0.0f, 0.0f};
        p2_Cards[i].collidable = true;
        UpdateCardCenter(&p2_Cards[i]);

        p2_Cards[i].card_Index = i;
        p2_Cards[i].stack_Value = casina_Board->player2.hand.cards[i]->value;
    }

    smax p1_Cards_Count = 4;
    SetupPlayerVisualBoardHand(visual_Board, p1_Cards, p1_Cards_Count, &visual_Board->player_One_Zone);
    for(umax i = 0; i < casina_Board->player1.hand.card_Count; ++i)
    {
        p1_Cards[i].center = {0.0f, 0.0f};
        p1_Cards[i].collidable = true;
        UpdateCardCenter(&p1_Cards[i]);
        
        p1_Cards[i].card_Index = i;
        p1_Cards[i].stack_Value = casina_Board->player1.hand.cards[i]->value;
    }
}

int main(int argc, char** argv)
{
    // Init memory
    SetupMemoryPools();

    // Setup system data
    System_State system_State = {};
    SystemStateSetup(&system_State);
    SetupRaylib(&system_State);

    // Setup game data
    Game_State game_State = {};

    Camera2D camera = {0};
    //camera.target = { 400, 280 };
    //camera.offset = { system_State.screen_Width/2.0f, system_State.screen_Height/2.0f };
    camera.target = { 0, 0 };
    camera.offset = { 0, 0 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Setup Casina Game
    CasinaBoard casina_Board = {};
    CreateCasinaPlayset(&casina_Board.playset);
    SetupCasinaGame(&casina_Board);
    StartCasinaGame(&casina_Board);
    Visual_Board visual_Board;
    SetupVisualBoard(&visual_Board, (real32)system_State.screen_Width, (real32)system_State.screen_Height);

    // Font info
    Font gui_Font = GuiGetFont();
    const char* working_Directory = GetWorkingDirectory();
    Vector2 font_Size = MeasureTextEx(gui_Font, working_Directory, gui_Font.baseSize, 0);

    float card_Height = 88.0f;
    float card_Width = 63.0f;

    Color base_Card_Color = MAROON;

    Visual_Card *p2_Cards = 0;
    smax p2_Cards_Count = 4;
    STORE_PERM_DATA(p2_Cards, p2_Cards_Count);
    SetupPlayerVisualBoardHand(&visual_Board, p2_Cards, p2_Cards_Count, &visual_Board.player_Two_Zone);
    for(umax i = 0; i < casina_Board.player2.hand.card_Count; ++i)
    {
        p2_Cards[i].center = {0.0f, 0.0f};
        p2_Cards[i].collidable = true;
        p2_Cards[i].color = base_Card_Color;
        p2_Cards[i].font_Height = font_Size.y;
        UpdateCardCenter(&p2_Cards[i]);

        p2_Cards[i].card_Index = i;
        p2_Cards[i].stack_Value = casina_Board.player2.hand.cards[i]->value;
    }

    smax p1_Cards_Count = 4;
    Visual_Card *p1_Cards = 0;
    STORE_PERM_DATA(p1_Cards, p1_Cards_Count);
    SetupPlayerVisualBoardHand(&visual_Board, p1_Cards, p1_Cards_Count, &visual_Board.player_One_Zone);
    for(umax i = 0; i < casina_Board.player1.hand.card_Count; ++i)
    {
        p1_Cards[i].center = {0.0f, 0.0f};
        p1_Cards[i].collidable = true;
        p1_Cards[i].color = base_Card_Color;
        p1_Cards[i].font_Height = font_Size.y;
        UpdateCardCenter(&p1_Cards[i]);
        
        p1_Cards[i].card_Index = i;
        p1_Cards[i].stack_Value = casina_Board.player1.hand.cards[i]->value;
    }

    smax board_Cards_Count = 4; //casina_Board.combo_Count
    smax max_Board_Cards_Count = 12;
    Visual_Card *board_Cards = 0;
    STORE_PERM_DATA(board_Cards, max_Board_Cards_Count);
    SetupGameBoard(&visual_Board, board_Cards, max_Board_Cards_Count);
    for(umax i = 0; i < board_Cards_Count; ++i)
    {
        board_Cards[i].center = {0.0f, 0.0f};
        board_Cards[i].collidable = true;
        board_Cards[i].color = base_Card_Color;
        board_Cards[i].font_Height = font_Size.y;
        UpdateCardCenter(&board_Cards[i]);

        board_Cards[i].card_Index = 0;
        board_Cards[i].stack_Value = casina_Board.combos[i].combo.cards[0]->value;
    }
    for(umax i = board_Cards_Count; i < max_Board_Cards_Count; ++i)
    {
        board_Cards[i].center = {0.0f, 0.0f};
        board_Cards[i].collidable = false;
        board_Cards[i].color = base_Card_Color;
        board_Cards[i].stack_Value = 0;
        board_Cards[i].card_Index = -1;
    }

    //----------------------------------------------------------------------------------

    // General variables
    Vector2 mousePosition = { 0.0f, 0.0f };
    Vector2 panOffset = mousePosition;

    Vector2 touchPosition = { 0.0f, 0.0f };
    Rectangle touchArea = { 0.0f, 0.0f, (float)system_State.screen_Width, (float)system_State.screen_Height };
    int currentGesture = GESTURE_NONE;
    int lastGesture = GESTURE_NONE;

    bool mouseScaleMode = false;
    bool mouseScaleReady = false;
    float window_Corner = 12.0f;
    
    Rectangle active_Rect = {0};
    float width_Corner =  0.0f;
    float height_Corner = 0.0f;
    bool active_ScaleReady = false;
    bool active_ScaleMode = false;

    bool drag_Player = false;
    Vector2 drag_Offset = {0};

    Debug_Window debug_Window = {};
    debug_Window.alive = false;
    debug_Window.previousTextBox = debug_Window.allTextBox[0];
    debug_Window.activeTextBox = debug_Window.allTextBox[0];
    debug_Window.activeTextBox[0] = '>';
    int frame_Counter = 0;

    CharArray command = {};
    command.char_Length = 0;
    command.allocated_Bytes = 512;
    STORE_PERM_DATA(command.chars, command.allocated_Bytes);

    CharArray token = {};
    token.char_Length = 0;
    token.allocated_Bytes = 512;
    STORE_PERM_DATA(token.chars, token.allocated_Bytes);

    Debug_Items scene_Debug_Items = {0};
    scene_Debug_Items.item_Count = 0;
    scene_Debug_Items.item_Hover = -1;
    scene_Debug_Items.item_Selected = -1;

    for(int i = 0; i < p1_Cards_Count; ++i)
    {
        scene_Debug_Items.items[scene_Debug_Items.item_Count] = { "P1 Card", &p1_Cards[i].rectangle, CARD_TYPE, (char*)&p1_Cards[i] };
        SetupDebugGuiData(&scene_Debug_Items.items[scene_Debug_Items.item_Count]);
        ++scene_Debug_Items.item_Count;
    }

    for(int i = 0; i < p2_Cards_Count; ++i)
    {
        scene_Debug_Items.items[scene_Debug_Items.item_Count] = { "P2 Card", &p2_Cards[i].rectangle, CARD_TYPE, (char*)&p2_Cards[i] };
        SetupDebugGuiData(&scene_Debug_Items.items[scene_Debug_Items.item_Count]);
        ++scene_Debug_Items.item_Count;
    }

    for(int i = 0; i < max_Board_Cards_Count; ++i)
    {
        scene_Debug_Items.items[scene_Debug_Items.item_Count] = { "Board Card", &board_Cards[i].rectangle, CARD_TYPE, (char*)&board_Cards[i] };
        SetupDebugGuiData(&scene_Debug_Items.items[scene_Debug_Items.item_Count]);
        ++scene_Debug_Items.item_Count;
    }

    // SetupDebugGuiData(&scene_Debug_Items.items[scene_Debug_Items.item_Count]);
    // ++scene_Debug_Items.item_Count;

    Debug_Items gui_Debug_Items = {0};
    gui_Debug_Items.item_Count = 0;
    gui_Debug_Items.item_Hover = -1;
    gui_Debug_Items.item_Selected = -1;


    Gui_Window editor_Window = {0};
    InitGuiWIndow(editor_Window);
    editor_Window.title = (char*)"#198# Editor Window";
    editor_Window.position = { system_State.screen_Width * 0.75f, 0.0f };
    editor_Window.rectangle = { 20.0f, 20.0f, (float)system_State.screen_Width / 2.0f, (float)system_State.screen_Height / 2.0f};
    editor_Window.open_Size.x = editor_Window.rectangle.width;
    editor_Window.open_Size.y = editor_Window.rectangle.height;
    // &editor_Window.open_Size
    gui_Debug_Items.items[gui_Debug_Items.item_Count] = { "Editor Window", &editor_Window.rectangle, WINDOW_TYPE, (char*)&editor_Window };
    SetupDebugGuiData(&gui_Debug_Items.items[gui_Debug_Items.item_Count]);
    ++gui_Debug_Items.item_Count;

    Color debug_Pink = {255, 0, 255, 255};
    Color debug_Green = {0, 255, 0, 255};
    Color debug_Yellow = {0, 255, 255, 255};
    int debug_Line_Size = 2;

    Debug_Item *active_Item = 0;
    Vector2 active_Test = {0};

    Vector2 hot_Spots[4] = {0};
    int hot_Spot_Count = 4;
    float hot_Spot_Size = 5.0f;

    bool isMouseLeftPressed = false;
    bool isMouseLeftReleased = false;

    smax card_Collision_Index = -1;

    // Main game loop
    while (!WindowShouldClose()) 
    {
        float deltaTime = GetFrameTime();

        card_Collision_Index = -1;

        if(casina_Board.player2.hand.card_Count == 0)
        {
            DrawOutRound(&casina_Board);
        }

        // detect input
        //----------------------------------------------------------------------------------
        if(IsKeyPressed(KEY_E)) // TOGGLE CONTROL MODE
        {
            if((game_State.editor_Control_Settings == EDITOR_CONTROL_EDIT))
            {
                game_State.editor_Control_Settings = EDITOR_CONTROL_PLAY;
            }
            else
            {
                game_State.editor_Control_Settings = EDITOR_CONTROL_EDIT;
            }
        }

        if(IsKeyPressed(KEY_R)) // RESET THE LEVEL
        {
            game_State.editor_Control_Settings = EDITOR_CONTROL_EDIT;
            camera.zoom = 1.0f;
        }

        if((game_State.editor_Control_Settings == EDITOR_CONTROL_EDIT))
        {
            if(IsKeyPressed(KEY_Q)) // TOGGLE EDIT MODE
            {
                if(game_State.editor_Mode_Settings == EDITOR_MODE_GAME)
                {
                    game_State.editor_Mode_Settings = EDITOR_MODE_UI;
                }
                else
                {
                    game_State.editor_Mode_Settings = EDITOR_MODE_GAME;
                }
            }

            if(IsKeyPressed(KEY_ESCAPE))
            {
                CloseWindow();
            }

            // Update Debug Menu
            if(IsKeyPressed('`') && IsKeyDown(KEY_LEFT_SHIFT))
            {
                debug_Window.alive = !debug_Window.alive;
            }
            if(IsKeyPressed(KEY_UP))
            {
                if(debug_Window.command_Count > 0)
                {
                    TextCopy(debug_Window.activeTextBox, debug_Window.previousTextBox);
                }
            }
        }

        isMouseLeftPressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        isMouseLeftReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

        lastGesture = currentGesture;
        currentGesture = GetGestureDetected();
        touchPosition = GetTouchPosition(0);
        Vector2 screen_Space_Touch_Position = GetScreenToWorld2D(touchPosition, camera);

        // update
        //----------------------------------------------------------------------------------
        //UpdateCameraPlayerBoundsPush(&camera, walls, walls_Length, deltaTime, system_State.screen_Width, system_State.screen_Height);

        scene_Debug_Items.item_Hover = -1;
        gui_Debug_Items.item_Hover = -1;

        // if(game_State.editor_Control_Settings == EDITOR_CONTROL_PLAY)
        // {
        // }
        // else if(game_State.editor_Control_Settings == EDITOR_CONTROL_EDIT)
        // {
            camera.zoom += ((float)GetMouseWheelMove()*0.05f);
            if (camera.zoom > 3.0f)
            {
                camera.zoom = 3.0f;
            }
            else if (camera.zoom < 0.25f)
            {
                camera.zoom = 0.25f;
            }

            //find the item a mouse is over
            if(game_State.editor_Mode_Settings == EDITOR_MODE_GAME)
            {
                for(int i = 0; i < scene_Debug_Items.item_Count; ++i)
                {
                    if(CheckCollisionPointRec(screen_Space_Touch_Position, *scene_Debug_Items.items[i].rect))
                    {
                        active_Test = screen_Space_Touch_Position;
                        scene_Debug_Items.item_Hover = i;
                    }
                }
            }
            else if(game_State.editor_Mode_Settings == EDITOR_MODE_UI)
            {
                for(int i = 0; i < gui_Debug_Items.item_Count; ++i)
                {
                    if(CheckCollisionPointRec(touchPosition, *gui_Debug_Items.items[i].rect))
                    {
                        active_Test = touchPosition;
                        gui_Debug_Items.item_Hover = i;
                    }
                }
            }
        // }

        if(isMouseLeftPressed)
        {
            if(CheckCollisionPointRec(touchPosition, { editor_Window.rectangle.x, editor_Window.rectangle.y, editor_Window.rectangle.width, 20.0f }))
            {
                panOffset.x = touchPosition.x - editor_Window.rectangle.x;
                panOffset.y = touchPosition.y - editor_Window.rectangle.y;

                if(currentGesture == GESTURE_DOUBLETAP)
                {
                    editor_Window.open = !editor_Window.open;
                }

                if(currentGesture == GESTURE_TAP)
                {
                    editor_Window.drag = true;
                }
            }


            if(CheckCollisionPointCircle(active_Test, hot_Spots[3], hot_Spot_Size))
            {
            }
            else
            {
                active_Item = 0;

                if(!CheckCollisionPointRec(touchPosition, editor_Window.rectangle))
                {
                    scene_Debug_Items.item_Selected = scene_Debug_Items.item_Hover;
                    gui_Debug_Items.item_Selected = gui_Debug_Items.item_Hover;
                    if(scene_Debug_Items.item_Selected > -1)
                    {
                        active_Item = &scene_Debug_Items.items[scene_Debug_Items.item_Selected];
                    }
                    else if(gui_Debug_Items.item_Selected > -1)
                    {
                        active_Item = &gui_Debug_Items.items[gui_Debug_Items.item_Selected];
                    }
                }
            }

            if(active_Item && CheckCollisionPointRec(screen_Space_Touch_Position, *active_Item->rect))
            {
                if(currentGesture == GESTURE_TAP)
                {
                    drag_Player = true;
                    drag_Offset = {screen_Space_Touch_Position.x - active_Item->rect->x, screen_Space_Touch_Position.y - active_Item->rect->y};
                }
            }
        }

        if(editor_Window.drag) 
        {
            editor_Window.rectangle.x = (touchPosition.x - panOffset.x);
            editor_Window.rectangle.y = (touchPosition.y - panOffset.y);
            
            if(isMouseLeftReleased)
            {
                editor_Window.drag = false;
            }
        }

        if(active_Item)
        {
            active_Rect = *active_Item->rect;

            hot_Spots[0] = {active_Rect.x, active_Rect.y};
            hot_Spots[1] = {active_Rect.x + active_Rect.width, active_Rect.y};
            hot_Spots[2] = {active_Rect.x, active_Rect.y + active_Rect.height};
            hot_Spots[3] = {active_Rect.x + active_Rect.width, active_Rect.y + active_Rect.height};

            if(CheckCollisionPointCircle(active_Test, hot_Spots[3], hot_Spot_Size))
            {
                active_ScaleReady = true;
                if(isMouseLeftPressed)
                {
                    active_ScaleMode = true;
                }
            }
            else
            {
                active_ScaleReady = false;
            }

            if(drag_Player && !active_ScaleMode)
            {
                //player.position.x = active_Test.x;
                //player.position.y = active_Test.y;
                MoveEntityData(active_Item, {active_Test.x - drag_Offset.x, active_Test.y - drag_Offset.y});

                if(active_Item->type == CARD_TYPE)
                {
                    UpdateCardCenter((Visual_Card*)active_Item->entity);
                }

                if(isMouseLeftReleased)
                {
                    active_Item = 0;
                    drag_Player = false;
                }
            }

            if(active_ScaleMode)
            {
                active_ScaleReady = true;

                active_Rect.width = (active_Test.x - active_Rect.x);
                active_Rect.height = (active_Test.y - active_Rect.y);

                if(active_Rect.width < 0.0f)
                {
                    active_Rect.width = 0.0f;
                }
                if(active_Rect.height < 0.0f)
                {
                    active_Rect.height = 0.0f;
                }

                *active_Item->rect = active_Rect;

                if(isMouseLeftReleased)
                {
                    active_ScaleMode = false;
                    drag_Player = false;
                }
            }
        }

        Visual_Card *selected_Card = 0;
        if(casina_Board.current_Player == GAME_PLAYER_ONE)
        {
            for(umax i = 0; i < p1_Cards_Count; ++i)
            {
                if(p1_Cards[i].collidable && CheckCollisionPointRec(touchPosition, p1_Cards[i].rectangle))
                {
                    selected_Card = &p1_Cards[i];
                    break;
                }
            }
        }
        else if(casina_Board.current_Player == GAME_PLAYER_TWO)
        {
            for(umax i = 0; i < p2_Cards_Count; ++i)
            {
                if(p2_Cards[i].collidable && CheckCollisionPointRec(touchPosition, p2_Cards[i].rectangle))
                {
                    selected_Card = &p2_Cards[i];
                    break;
                }
            }
        }

        // Release a card being drug
        if(selected_Card && isMouseLeftReleased)
        {
            bool stack_Card = false;
            bool score_Card = false;
            smax stack_Total = selected_Card->stack_Value;
            bool value_On_Board = false;

            card_Collision_Index = ActiveCardBoardCollision(selected_Card, board_Cards, board_Cards_Count);
            Visual_Card *player_Cards = (casina_Board.current_Player == GAME_PLAYER_ONE) ? p1_Cards: p2_Cards;
            Rectangle *player_Zone = (casina_Board.current_Player == GAME_PLAYER_ONE) ? &visual_Board.player_One_Zone : &visual_Board.player_Two_Zone;

            if(card_Collision_Index >= 0) // If hand card collides with board card
            {
                if((selected_Card->stack_Value + board_Cards[card_Collision_Index].stack_Value) <= 10)
                {
                    stack_Card = true;
                    stack_Total = selected_Card->stack_Value + board_Cards[card_Collision_Index].stack_Value;
                }

                if(stack_Total == board_Cards[card_Collision_Index].stack_Value)
                {
                    score_Card = true;
                }
            }
            else if(CheckCollisionRecs(selected_Card->rectangle, visual_Board.play_Zone)) // If hand card collides with the play area
            {
                for(smax i = 0; i < board_Cards_Count; ++i)
                {
                    if(selected_Card->stack_Value == board_Cards[i].stack_Value)
                    {
                        score_Card = true;
                        value_On_Board = true;
                        ScoreBoardCard(&casina_Board, i);
                        
                        board_Cards[i].collidable = false;
                        board_Cards[i].card_Index = -1;
                        board_Cards[i].stack_Value = 0;
                        SwapVisualCards(board_Cards + i, board_Cards + (board_Cards_Count-1));
                        --board_Cards_Count;
                        i = 0;
                    }
                }

                if(!score_Card)
                {
                    stack_Card = true;
                }
            }
            else // If card collides with nothing
            {
                SetupGameBoard(&visual_Board, board_Cards, 12);
            }

            if(stack_Card && !score_Card)
            {
                // Move Card
                if(card_Collision_Index > -1)
                {
                    StackCard(&casina_Board, selected_Card->card_Index, card_Collision_Index);
                }
                else
                {
                    StackCard(&casina_Board, selected_Card->card_Index);

                    smax new_Pos = board_Cards_Count;
                    if(casina_Board.combo_Count < board_Cards_Count)
                    {
                        new_Pos = casina_Board.combo_Count-1;
                    }
                    else
                    {
                        ++board_Cards_Count;
                    }

                    board_Cards[new_Pos].collidable = true;
                    board_Cards[new_Pos].card_Index = 0;
                    board_Cards[new_Pos].stack_Value = selected_Card->stack_Value;
                }


                // Update old cards
                UpdateVisualCards(player_Cards, selected_Card->card_Index + 1, 4);

                // Clean up
                card_Collision_Index = -1;
                selected_Card->collidable = false;
                selected_Card->card_Index = -1;
                SwitchCurrentPlayer(&casina_Board);
            }

            if(score_Card)
            {
                ScoreHandCard(&casina_Board, selected_Card->card_Index);
                casina_Board.last_To_Score = casina_Board.current_Player;

                if(card_Collision_Index > -1)
                {
                    ScoreBoardCard(&casina_Board, card_Collision_Index);
                }

                // Update old cards
                UpdateVisualCards(player_Cards, selected_Card->card_Index + 1, 4);

                // Clean up
                card_Collision_Index = -1;
                selected_Card->collidable = false;
                selected_Card->card_Index = -1;
                SwitchCurrentPlayer(&casina_Board);
            }

            SetupPlayerVisualBoardHand(&visual_Board, player_Cards, 4, player_Zone);
        }

        if(RoundOver(&casina_Board))
        {
            // TODO Handle Game Over Better
            if(casina_Board.player1.hand.card_Count > 0)
            {
                SetupPlayerVisualBoardHand(&visual_Board, p1_Cards, 4, &visual_Board.player_One_Zone);
                SetupPlayerVisualBoardHand(&visual_Board, p2_Cards, 4, &visual_Board.player_Two_Zone);
                SetupGameBoard(&visual_Board, board_Cards, 12);
                SetupVisualCards(&casina_Board, &visual_Board, p1_Cards, p2_Cards);
            }
        }

        if(editor_Window.open)
        {
            editor_Window.rectangle.width = editor_Window.open_Size.x;
            editor_Window.rectangle.height = editor_Window.open_Size.y;
        }
        else
        {
            editor_Window.rectangle.height = 1.0f;
        }

        // draw
        //----------------------------------------------------------------------------------

        BeginDrawing();

            ClearBackground(system_State.clear_Color);

            DrawRectangleRec({0.0f, 0.0f, (float)system_State.screen_Width, (float)system_State.screen_Height}, BLACK);

            BeginMode2D(camera);

                GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

                DrawRectangleRec(visual_Board.player_Two_Zone, LIGHTGRAY);
                DrawRectangleRec(visual_Board.play_Zone, DARKGRAY);
                DrawRectangleRec(visual_Board.player_One_Zone, LIGHTGRAY);

                DrawVisualCards(p2_Cards, &casina_Board.player2.hand, 4);
                for(int i = 0; i < board_Cards_Count; ++i)
                {
                    if(casina_Board.combos[i].combo.card_Count)
                    {
                        DrawVisualCards(board_Cards + i, &casina_Board.combos[i].combo, 1);
                    }
                }
                DrawVisualCards(p1_Cards, &casina_Board.player1.hand, 4);

            EndMode2D();

            editor_Window.clicked - GuiWindowBox(editor_Window.rectangle, editor_Window.title);
            if(editor_Window.open)
            {
                if(scene_Debug_Items.item_Selected > -1)
                {
                    DebugEditor(&editor_Window, &scene_Debug_Items.items[scene_Debug_Items.item_Selected]);
                }
                else if(gui_Debug_Items.item_Selected > -1)
                {
                    DebugEditor(&editor_Window, &gui_Debug_Items.items[gui_Debug_Items.item_Selected]);
                }
            }

            if(debug_Window.alive)
            {
                //GuiLock();

                debug_Window.window = {0.0f, 0.0f, (float)system_State.screen_Width, (float)system_State.screen_Height / 2.0f};
                DrawRectangleRec(debug_Window.window, { 80, 80, 80, 255 });

                // Path Label
                GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
                Rectangle label_Rect = {debug_Window.window.x, debug_Window.window.y, debug_Window.window.width, font_Size.y};
                GuiLabel(label_Rect, working_Directory);

                // Text box
                GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
                debug_Window.text_Box = {debug_Window.window.x, debug_Window.window.height - 10.0f - font_Size.y, debug_Window.window.width, font_Size.y + 10.0f};
                if(GuiCommandBox(debug_Window.text_Box, debug_Window.activeTextBox, 64, debug_Window.alive, &frame_Counter))
                {
                    if(debug_Window.command_Count < 10)
                    {
                        //CopyString(debug_Window.activeTextBox, &command);
                        TextCopy(command.chars, debug_Window.activeTextBox);

                        // if((command.length > 0) &&  (command.string[0] == '>'))
                        // {
                        //     RemoveFront(&command);

                        //     while((command.length > 0) && IsWhiteSpace(command.string[0]))
                        //     {
                        //         RemoveFront(&command);
                        //     }

                        //     while((command.length > 0) && !IsWhiteSpace(command.string[0]))
                        //     {
                        //         AddChar(&token, command.string[0]);
                        //         RemoveFront(&command);
                        //     }
                        // }

                        if((command.char_Length > 0) &&  (command.chars[0] == '>'))
                        {
                            RemoveFront(&command);

                            while((command.char_Length > 0) && IsWhiteSpace(&command))
                            {
                                RemoveFront(&command);
                            }

                            while((command.char_Length > 0) && !IsWhiteSpace(&command))
                            {
                                AddChar(&token, command.chars[0]);
                                RemoveFront(&command);
                            }
                        }

                        ++debug_Window.command_Count;
                        debug_Window.previousTextBox = debug_Window.allTextBox[debug_Window.command_Count-1];
                        debug_Window.activeTextBox = debug_Window.allTextBox[debug_Window.command_Count];
                        debug_Window.activeTextBox[0] = '>';
                    }
                }

                GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
                for(int i = 0; i < debug_Window.command_Count; ++i)
                {
                    float step1 = debug_Window.command_Count - i;
                    float step2 = step1 + 1;
                    Rectangle label_OldCommand = {debug_Window.window.x, debug_Window.window.height - 10.0f * step2 - font_Size.y * step2, debug_Window.window.width, font_Size.y * step1 - 10.0f * step1};
                    GuiLabel(label_OldCommand, debug_Window.allTextBox[i]);
                }
                //GuiUnlock();
            }

            if(game_State.editor_Mode_Settings == EDITOR_MODE_GAME)
            {
                BeginMode2D(camera);

                if(scene_Debug_Items.item_Hover > -1)
                {
                    DrawRectangleLinesEx(*scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect, debug_Line_Size, debug_Green);
                    DrawLineEx({scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect->x, scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect->y}, {scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect->x + scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect->width, scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect->y + scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect->height}, debug_Line_Size, debug_Green);
                }

                if(scene_Debug_Items.item_Selected > -1)
                {
                    DrawCircleV(hot_Spots[0], hot_Spot_Size, YELLOW);
                    DrawCircleV(hot_Spots[1], hot_Spot_Size, YELLOW);
                    DrawCircleV(hot_Spots[2], hot_Spot_Size, YELLOW);
                    DrawCircleV(hot_Spots[3], hot_Spot_Size, YELLOW);
                    DrawRectangleLinesEx(*scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect, debug_Line_Size, debug_Yellow);
                    DrawLineEx({scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect->x, scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect->y}, {scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect->x + scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect->width, scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect->y + scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect->height}, debug_Line_Size, debug_Yellow);
                }

                // if(card_Collision_Index > -1)
                // {
                //     DrawRectangleLinesEx(*active_Item->rect, debug_Line_Size, debug_Green);
                //     DrawRectangleLinesEx(board_Cards[card_Collision_Index].rectangle, debug_Line_Size, debug_Green);
                //     board_Cards[card_Collision_Index].color = BLUE;
                // }

                EndMode2D();
            }
            else if(game_State.editor_Mode_Settings == EDITOR_MODE_UI)
            {
                if(gui_Debug_Items.item_Hover > -1)
                {
                    DrawRectangleLinesEx(*gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect, debug_Line_Size, debug_Green);
                    DrawLineEx({gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect->x, gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect->y}, {gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect->x + gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect->width, gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect->y + gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect->height}, debug_Line_Size, debug_Green);
                }

                if(gui_Debug_Items.item_Selected > -1)
                {
                    DrawCircleV(hot_Spots[0], hot_Spot_Size, YELLOW);
                    DrawCircleV(hot_Spots[1], hot_Spot_Size, YELLOW);
                    DrawCircleV(hot_Spots[2], hot_Spot_Size, YELLOW);
                    DrawCircleV(hot_Spots[3], hot_Spot_Size, YELLOW);
                    DrawRectangleLinesEx(*gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect, debug_Line_Size, debug_Yellow);
                    DrawLineEx({gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect->x, gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect->y}, {gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect->x + gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect->width, gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect->y + gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect->height}, debug_Line_Size, debug_Yellow);
                }
            }

            //DEBUG_DrawLines();

            DrawFPS(system_State.screen_Width - 90, system_State.screen_Height - 30);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CleanupRaylib();

    return 0;
}