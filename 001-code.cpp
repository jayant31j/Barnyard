#include "splashkit.h"
#include "chick_farm.h"
#include "eggs.h"
#include "graphics_game.h"
#include <vector>

using namespace std;

#define SCREEN_BORDER 75

/**
 * Load the game images, sounds, etc.
 */
void load_resources()
{
    load_resource_bundle("game_bundle", "farm_house.txt");
}

/**
 * Draw the eggs present in farmer
 * 
 * @param farmer farmer_data to assess the dyn. array of eggs
 */
void draw_eggs(const farmer_data &farmer)
{
    for (int i = 0; i < farmer.eggs.size(); i++)
    {
        draw_egg(farmer.eggs[i]);
    }
}

/**
 * Update the eggs present in farmer
 * 
 * @param farmer farmer_data to assess the dyn. array of eggs
 */
void update_eggs(farmer_data &farmer)
{
    for (int i = 0; i < farmer.eggs.size(); i++)
    {
        update_egg(farmer.eggs[i]);
    }
}

/**
 * Creates a new farmer to play the game
 */
farmer_data new_farmer()
{
    farmer_data farmer;
    farmer.basket = new_basket();

    return farmer;
}

/**
 * Entry point.
 * 
 * Manages the initialisation of data, the event loop, and quitting.
 */
int main()
{
    open_window("Farm house", 800, 600);

    load_resources();

    draw_bitmap("background", 0, 0);
    play_sound_effect("background_sound", 10, 0.1);
    refresh_screen(60);
    delay(3000);

    farmer_data farmer;
    farmer = new_farmer();

    while (not quit_requested())
    {
        //create a new egg once the previous egg is gone
        if (farmer.eggs.size() <= 0)
        {
            farmer.eggs.push_back(new_egg(rnd(800 - SCREEN_BORDER)));
        }

        // Handle input to adjust my_basket movement
        process_events();
        handle_input(farmer.basket);

        // Perform movement and update the camera
        update_basket(farmer.basket);
        update_eggs(farmer);

        //Give background to the game
        draw_backdrop();
        draw_scoreboard(farmer);

        //as well as the my_basket who can move
        draw_basket(farmer.basket);
        draw_eggs(farmer);

        check_collision(farmer);
        crack_eggs(farmer);

        refresh_screen(60);

        //to end game once all lives are lost
        if (farmer.basket.lives <= 0)
        {
            //showing a game over sign
            game_end_backdrop();
            break;
        }
    }

    //End the game and close window
    close_window("Farm house");

    return 0;
}

/**
 * chick_farm.cpp
 */

#define SCREEN_BORDER 1
#define SPEED 0.2

basket_data new_basket()
{
    basket_data my_basket;
    bitmap default_bitmap = bitmap_named("basket");

    // Create the sprite with 3 layers - we can turn on and off based
    // on the ship kind selected
    my_basket.basket_sprite = create_sprite(default_bitmap);

    // Position in the centre of the initial screen
    sprite_set_x(my_basket.basket_sprite, 400);
    sprite_set_y(my_basket.basket_sprite, 500);

    return my_basket;
}

void draw_basket(const basket_data &basket_to_draw)
{
    draw_sprite(basket_to_draw.basket_sprite);
}

void update_basket(basket_data &basket_to_update)
{
    // Apply movement based on rotation and velocity in the sprite
    update_sprite(basket_to_update.basket_sprite);
}

void handle_input(basket_data &my_basket)
{
    // Handle movement - rotating left/right and moving forward/back
    float dx = sprite_dx(my_basket.basket_sprite);

    // Allow rotation with left/right keys
    if (key_down(LEFT_KEY))
        sprite_set_dx(my_basket.basket_sprite, dx - SPEED);
    if (key_down(RIGHT_KEY))
        sprite_set_dx(my_basket.basket_sprite, dx + SPEED);

    point_2d location = center_point(my_basket.basket_sprite);

    //starts reversing the direction in case basket move out of screen area to get it back
    if (location.x >= screen_width() - 0.5 * bitmap_width("basket"))
    {
        sprite_set_x(my_basket.basket_sprite, screen_width() - bitmap_width("basket"));
        sprite_set_dx(my_basket.basket_sprite, -1 * SPEED);
    }

    //starts reversing the direction in case basket move out of screen area to get it back
    if (location.x <= 0)
    {
        sprite_set_x(my_basket.basket_sprite, bitmap_width("basket"));
        sprite_set_dx(my_basket.basket_sprite, SPEED);
    }
}

//If eggs and basket has collided the effect are produced
void check_collision(farmer_data &farmer)
{
    point_2d location;
    for (int i = 0; i < farmer.eggs.size(); i++)
    {
        location = center_point(farmer.eggs[i].egg_sprite);
        if (sprite_collision(farmer.eggs[i].egg_sprite, farmer.basket.basket_sprite))
        {
            play_sound_effect("pop");
            add_to_basket(farmer, i);
            remove_egg(farmer, i);
        }
    }
}

//if the egg and basket hasn't collided
void crack_eggs(farmer_data &farmer)
{
    point_2d location;

    for (int i = 0; i < farmer.eggs.size(); i++)
    {
        location = center_point(farmer.eggs[i].egg_sprite);

        if (location.y > 550)
        {
            play_sound_effect("crack");
            remove_egg(farmer, i);
            draw_bitmap("cracked_egg", location.x - 25, 530);
            refresh_screen(120);
            delay(500);
            farmer.basket.lives = farmer.basket.lives - 1;
        }
    }
}

// removes the eggs from the vector possessed by the farmer struct
void remove_egg(farmer_data &farmer, int idx)
{
    if (idx >= 0 && idx < farmer.eggs.size())
    {
        int last = farmer.eggs.size() - 1;
        farmer.eggs[idx] = farmer.eggs[last];
        farmer.eggs.pop_back();
    }
}

void add_to_basket(farmer_data &farmer, int idx)
{
    switch (farmer.eggs[idx].kind)
    {
    case GOLD:
        farmer.basket.gold_eggs = farmer.basket.gold_eggs + 1;
        farmer.basket.score = farmer.basket.score + 3;
        break;
    case SILVER:
        farmer.basket.silver_eggs = farmer.basket.silver_eggs + 1;
        farmer.basket.score = farmer.basket.score + 2;
        break;
    case REGULAR:
        farmer.basket.reg_eggs = farmer.basket.reg_eggs + 1;
        farmer.basket.score = farmer.basket.score + 1;
        break;
    default:
        farmer.basket.score = farmer.basket.score + 1;
        break;
    }
}

/**
 * chick_farm.h
 */


/**
 * basket is the entity using which the game is played and it would 
 * poscess the points and eggs 
 * 
 * @field basket_sprite - sprite of the basket
 * @field score - score of the player
 * @field lives - there are initially 7, decrease after every one egg lost
 * @field gold_eggs - number of gold eggs collected
 * @field silver_eggs - number of silver eggs collected
 * @field reg_eggs - number of white eggs collected
 */
struct basket_data
{
   sprite basket_sprite;
   int score = 0;
   int lives = 7;
   int gold_eggs = 0;
   int silver_eggs = 0;
   int reg_eggs = 0;
};

/**
 * Conatins the eggs and the basket
 * 
 * @field basket - the entity to collect eggs
 * @field eggs- the list of eggs in the game
 */
struct farmer_data
{
   basket_data basket;
   vector<egg_data> eggs;
};

/**
 * Create a new basket 
 *  
 * @return basket_data type new basket
 */
basket_data new_basket();

/**
 * Draw the sprite of the basket
 * 
 * @param basket_to_draw basket to be drawn     
 */
void draw_basket(const basket_data &basket_to_draw);

/**
 * Update the sprite of the basket
 * 
 * @param basket_to_update basket to be updated   
 */
void update_basket(basket_data &basket_to_update);

/**
 * Control the sprite of the basket using keyboard
 * 
 * @param my_basket basket to be controlled     
 */
void handle_input(basket_data &my_basket);

/**
 * Draw the sprite of the basket
 * 
 * @param farmer farmer_data wherein collsions are to be checked 
 */
void check_collision(farmer_data &farmer);

/**
 * Delete eggs and decrease lives once the egg is not catched
 * using basket
 * 
 * @param farmer farmer_data wherein collsions are to be checked  
 */
void crack_eggs(farmer_data &farmer);

/**
 * Removes the eggs from the vector possessed by the farmer struct
 * 
 * @param farmer game_data variable posessing egg and basket
 * 
 * @param i index of the egg to be removed
 */
void remove_egg(farmer_data &farmer, int i);

/**
 * Affect the scores on the basis of the eggs catched
 * 
 * @param farmer game_data variable posessing eggs and basket
 * 
 * @param i index of the egg that affects scores
 */
void add_to_basket(farmer_data &farmer, int i);


/**
 * eggs.cpp
 */


#define SCREEN_BORDER1 10
#define EGG_DROP_VEL 1.5

/**
 * Converts a egg kind into a bitmap that can be used.
 * 
 * @param kind  The kind of ship you want the bitmap of
 * @return      The bitmap matching this egg kind
 */
bitmap egg_bitmap(egg_kind kind)
{
    switch (kind)
    {
    case REGULAR:
        return bitmap_named("regular");
    case SILVER:
        return bitmap_named("silver");
    default:
        return bitmap_named("gold");
    }
}

//Draw the image of the egg
egg_data new_egg(double x)
{
    egg_data egg;
    egg_kind  new_egg_kind;
    new_egg_kind = egg.kind = static_cast<egg_kind>(rnd(3));

    // Create the sprite
    // based on the powerup kind randomly selected
    egg.egg_sprite = create_sprite(egg_bitmap(new_egg_kind));

    // Position randomly at the screen screen
    sprite_set_x(egg.egg_sprite, x);
    sprite_set_y(egg.egg_sprite, 240);

    if(x < 0 or x > screen_width())
    {
        x = 300;
    }

    //provides a velocity to powerup
    sprite_set_dx(egg.egg_sprite, 0);
    sprite_set_dy(egg.egg_sprite, EGG_DROP_VEL);

    return egg;
}

//Draw the image of the egg
void draw_egg(const egg_data &egg_image)
{
    //after selecting the powerup printing its image on the screen
    draw_sprite(egg_image.egg_sprite);
}

// Update the sprite of the egg
void update_egg(egg_data &egg_to_update)
{  
    // Apply movement based on rotation and velocity in the sprite
    update_sprite(egg_to_update.egg_sprite);
}

/**
 * eggs.h
 */


/**
 * Different options for the kind of EGG.
 * Adjusts the image used.
 * 
 * @field REGULAR- for white egg
 * @field GOLD - for golden egg
 * @field SILVER - for silver egg
 * 
 */
enum egg_kind
{
    REGULAR,
    GOLD,
    SILVER
};

/**
 *Possessing a type and sprite to move and check collision
 * 
 * @field egg_kind- the type of egg from (reguler,gold,silver)
 * @field egg_sprite- sprite of the egg image
 * 
 */
struct egg_data
{
    egg_kind kind;
    sprite egg_sprite;
};

/**
 * Creates a new egg_data type variable
 * 
 * @param x x coordinate of the egg     
 * @return egg_data type new egg
 */
egg_data new_egg(double x);

/**
 * Draw the image of the egg
 * 
 * @param egg_image egg whose bitmap is to be drawn 
 */
void draw_egg(const egg_data &egg_image);

/**
 * Update the sprite of the egg
 * 
 * @param egg_to_update egg whose sprite is to be updated
 */
void update_egg(egg_data &egg_image);


/**
 * graphics_game.cpp
 */

void draw_backdrop()
{
    // Redraw everything
    clear_screen(COLOR_BLACK);

    draw_bitmap("barn", -200, 0);

    int x, y;
    y = 100;
    
    for ( x = 40; x < screen_width() - 10; x = x + 150)
    {
        draw_bitmap("chicken", x, y);
    }

    fill_rectangle(COLOR_ORANGE, 0, 0, 800, 50);
    
    draw_text("SCORE: ", COLOR_WHITE, "font1" , 20, 0, 0);

}

void game_end_backdrop()
{
    // Redraw everything
    clear_screen(COLOR_YELLOW);

    draw_bitmap("game_over", 0, 0);
    refresh_screen(60);
    delay(2000);

}

/**
 * Shows the points scored and eggs collected
 * Depicts progress of the program
 * 
 * @param farmer farmer to play the game
 */
void draw_scoreboard(const farmer_data &farmer)
{
    //score
    draw_text(to_string(farmer.basket.score), COLOR_WHITE, "font1", 20, 50, 0);

    //no. of regular eggs
    draw_bitmap("regular", 125, 5);
    draw_text(to_string(farmer.basket.reg_eggs), COLOR_BLACK, "font1", 20, 175, 0);
    //no. of silver eggs
    draw_bitmap("silver", 250, 5);
    draw_text(to_string(farmer.basket.silver_eggs), COLOR_BLACK, "font1", 20, 300, 0);
    //no. of golden eggs
    draw_bitmap("gold", 375, 5);
    draw_text(to_string(farmer.basket.gold_eggs), COLOR_BLACK, "font1", 20, 425, 0);

    for (int i = 0; i < farmer.basket.lives; i++)
    {
        draw_bitmap("heart", 500 + i * 35, 10);
    }
}

/**
 * graphics_game.h
 */


/**
 * Draw images for the game background
 */
void draw_backdrop();

/**
 * Draw images for the game over background
 */
void game_end_backdrop();

/**
 * Shows the points scored and eggs collected
 * Depicts progress of the program
 * 
 * @param farmer farmer to play the game
 */
void draw_scoreboard(const farmer_data &farmer);
