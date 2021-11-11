#pragma once

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