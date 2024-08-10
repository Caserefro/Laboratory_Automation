#ifndef BUTTON_LAYOUT_H
#define BUTTON_LAYOUT_H

Layout Button_Layout[MAX_GROUPS] = {
  { HomeScreenID, 5, {
                       //Home Screen
                       //lock unlock, Ac Controller, Weather, borrowed, Club desc
                       { 250, 166, 55, 56, ClubDesc_btn },            // Club desc popup  //known error: btn hitbox CAN NOT OVERLAP.
                       { 46, 44, 182, 120, LockUnlock_btn },          // lock unlock
                       { 252, 44, 182, 120, Ac_ScreenUpdate },        // Ac Controller Scr
                       { 46, 183, 182, 120, Weather_Screen },         // Weather Scr
                       { 252, 183, 182, 120, BorrowedScr_btn }  // borrowed Scr
                     } },
  { InfoPopupID, 1, {
                      //Info PopUp
                      //lock unlock, Ac Controller, Weather, borrowed, Club desc
                      { 325, 128, 95, 95, Home_ScreenUpdate }  // home
                    } },
  { BorrowedScreenID, 6, {
                           // Borrowed Screen
                           // button returned/not ret, home, left arrow, right arrow, save
                           { 0, 129, 76, 94, Home_ScreenUpdate },     // home
                           { 350, 10, 103, 103, BorrowedUp_btn },     // Borrowed Up btn
                           { 350, 207, 103, 103, BorrowedDown_btn },  // Borrowed down btn
                           { 123, 131, 64, 58, LeftArrow_btn },       // left arrow
                           { 278, 131, 64, 58, RightArrow_btn },      // right arrow
                           { 380, 128, 64, 64, Save_btn }             // save
                         } },
  { AcScreenID, 5, {
                     // Ac Screen
                     //button on/off, swing fan, home, down
                     { 192, 112, 95, 95, Home_ScreenUpdate },  // home
                     { 35, 23, 192, 127, OnOff_btn },          // button on/off
                     { 36, 170, 192, 127, SwingFan_btn },      // swing fan
                     { 308, 42, 140, 100, UpTemp_btn },        // Up
                     { 306, 178, 142, 100, DownTemp_btn }      // down
                   } },
  { WeatherScreenID, 1,                            //Weather Screen
    { { 394, 112, 85, 95, Home_ScreenUpdate } } }  // home

};
#endif