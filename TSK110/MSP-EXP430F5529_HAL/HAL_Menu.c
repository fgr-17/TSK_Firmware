#include "msp430.h"
#include "HAL_Dogs102x6.h"
#include "HAL_Buttons.h"
#include "HAL_Menu.h"
#include "HAL_Wheel.h"


/***************************************************************************//**
 * @brief   Set up a menu
 * @param   menuText  The text for the menu title and options.
 * @param   numItems  The number of menu items ( numItems <= 7 )
 * @return  Position of selection
 ******************************************************************************/

uint8_t Menu_active(char **menuText, uint8_t numItems)
{
    uint8_t i;
    uint8_t position = 0;
    uint8_t lastPosition = 9;

    if (numItems > 7){                                              // Screen is 8 lines tall = 1
                                                                    // title line + 7 items max
        numItems = 7;
    }
    Dogs102x6_clearImage(numItems + 1, 102, 0, 0);
    Dogs102x6_stringDraw(0, 0, menuText[0], DOGS102x6_DRAW_NORMAL); // Print the title

    buttonsPressed = 0;
    while (!buttonsPressed)                                         // Menu active until selection
                                                                    // is made
    {
        position = Wheel_getPosition();
        if (position > numItems){
            position = numItems;
        }
        else if (position == 0){
            position = 1;
        }
        if (position != lastPosition)                               // Update position if it is
                                                                    // changed
        {
            for (i = 1; i < numItems + 1; i++)                      // Display menu items
            {
                if (i != position){
                    Dogs102x6_stringDraw(i, 0, menuText[i], DOGS102x6_DRAW_NORMAL);
                }
                else {
                    // Highlight item at current position
                    Dogs102x6_stringDraw(i, 0, menuText[i], DOGS102x6_DRAW_INVERT);
                }
            }
            lastPosition = position;
        }  
    }
    
    return position;
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
