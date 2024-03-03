# Style Guide
[TOC]
This is a guide for writing clear examples that can be read by beginners and advanced users alike. You should abide by this guide unless there is an ariticulable reason to deviate from this guide. This is not a set of hard and fast rules, it's a set of guidelines. Some of these guidelines might even conflict with each other. Use your judgment on when they're best followed, and if you're not sure, ask a project maintainer or fellow contributor.

## Writing Example Code
* Efficiency is not paramount; readability is. 
* The most important users of OpenHornet are (relative) beginners and people who don't care about code, but about getting projects done. 
* Think generously about people who know less than you about code. Don't think they should understand some technical concept. They don't, and they're not stupid for not understanding. Your code should explain itself, or use comments to do the same. If it needs a complex concept like registers or interrupts or pointers, either explain it or skip it. 
* When forced to choose between technically simple and technically efficient, choose the former. 
* Introduce concepts only when they are useful and try to minimize the number of new concepts you introduce in each example. For example, at the very beginning, you can explain simple functions with no variable types other than int, nor for consts to define pin numbers. On the other hand, in an intermediate example, you might want to introduce peripheral concepts as they become useful.
* Concepts like using `const ints` to define pin numbers, choosing bytes over ints when you don't need more than 0 - 255, etc. are useful, but not central to getting started. So use them sparingly, and explain them when they're new to your lesson plan. 
* Put your `setup()` and your `loop()` at the beginning of the program. They help beginners to get an overview of the program, since all other functions are called from those two. 
* Use a verbose title block at the beginning of every sketch: 
```        
    /**
     * @file 4A3A1-SELECT_JETT_PANEL.ino
     * @author Arribe
     * @date 03.01.2024
     * @version 0.0.1
     * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
     * @brief Controls the SELECT JETT panel.
     *
     * @details
     * 
     *  * **Reference Designator:** 4A3A1
     *  * **Intended Board:** ABSIS ALE w/ Relay Module
     *  * **RS485 Bus Address:** 2
     * 
     * ### Wiring diagram:
     * PIN | Function
     * --- | ---
     * A0  | Hook Bypass Switch
     * A1  | Launch Bar Switch
     * A2  | Flaps Switch Position 2
     * A3  | Flaps Switch Position 1
     * 2   | Launch Bar Mag Switch
     * 3   | Hook Bypass Mag Switch
     * 4   | Landing Taxi Light
     * 6   | Selector Jettison Position 1
     * 7   | Selector Jettison Position 3
     * 8   | Selector Jettison Position 8
     * 10  | Antiskid Switch
     * 14  | Selector Jettison Position 2
     * 15  | Selector Jettison Push
     * 16  | Selector Jettison Position 4
    */
```

## Commenting Your Code
* All comments should use doxygen-compatible comments and tags.
### Doxygen Tags
* `@note` is the most generic tag, and is the one to use in most cases where you want the reader to "take notice" of the matter described in the section.
* `@attention` tag can be used to highlight a particularly important note, one that you don't want the reader to overlook.
* `@warning` tag should be used instead of `@attention` when there may be negative consequences to consider if the reader isn't careful in how he uses the item being documented.
* `@remark` and `@remarks` tags can be used for notes that have lesser importance. If you want to describe something in an "oh, by the way" sense, the remark tag is useful for that.
* `@todo` tag is used differently than the others you listed. It is usually used to indicate that the code being described in the comment has one or more unfinished aspects. This alerts both code users and code writers that a feature or bug needs to be addressed in a later revision of the pertinent section of code. Doxygen has a cool feature where it will list all TODOs together in their own section in the generated output.
* `@bug` tag also parses those comments into a separate buglist, much in a similar fashion to the `@todo` tag.

### Variables, Constants, and Macros/Defines
* Comment every variable or constant declaration with a description of what the variable does. Use trailing doxygen compatible comments unless it is complex enough to require a leading/trailing block comment. You may use markdown compatible comments within the doxygen comments.
    ```
    /**
    * @brief Pilots may want the launch bar to automatically release when the throttles advance to MIL power.
    * The sim's version of the F/A-18 does *NOT* have this feature.  The define allows setting it as desired, default is *false*.
    *
    */
    #define LBAR_SW_AUTORETRACT false

    // Define pins for DCS-BIOS per interconnect diagram.
    #define HOOKB_SW A0 ///< Hook Bypass Switch
    #define LBAR_SW A1 ///< Launch Bar Switch
    #define FLAPS_SW2 A2 ///< Flaps Switch Position 2
    #define FLAPS_SW1 A3 ///< Flaps Switch Position 1
    #define LBAR_RET 2    ///< Launch Bar Switch Mag

    //Declare variables for custom non-DCSBios logic for mag-switches
    bool hookLeverState = LOW;        ///< Initializing for correct cold/ground start position as switch releases on power off.
    unsigned long hookLeverTime = 0;  ///< Initializing variable to hold time of last hook change.
    bool wowLeft = true;              ///< Initializing weight-on-wheel value for cold/ground start.
    ```

### Code Blocks
* Comment every code block. Do it before the block if possible, so the reader knows what's coming.
    ```
    /**
    * @brief Need to save the hook lever state to test turning off the hook bypass mag-switch in the main loop.
    *  
    */
    void onHookLeverChange(unsigned int newValue) {
      hookLeverState = newValue;
      hookLeverTime = millis();
    } DcsBios::IntegerBuffer hookLeverBuffer(0x74a0, 0x0200, 9, onHookLeverChange);
    ```
* Comment every for loop

### Code Logic
* Use verbose if statements. For simplicity to the beginning reader, use the block format for everything, i.e. avoid this: 
<br><br>`if (somethingIsTrue) doSomething;`

    Instead, use this: 
    ```
    if (somethingIsTrue == TRUE) {
       doSomething;
    }
    ```
* Avoid pointers. They are confusing to novice programmers.
* Example explaining logic before code:
    ```
	/**
     ### Launch Bar Auto Retract Logic
    *  If the launch bar mag-switch is held in extend position, then: \n
    *  -# if no weight on wheels retract the launch bar. \n
    *  -# if the launch bar auto-retract is defined as true and both engines' RPM is >85 then retract launch bar. \n
    *   @note If launch bar auto-retract is true, when connecting to the catapault it may be easier to keep one engine under 80% while advancing the other with enough power get over the shuttle.
    * 
    */
      if (launchBarMagState == HIGH) {
        switch (launchBarState) {
          case LOW:  //launch bar switch in retract
            //do nothing the state change method will retract the launch bar
            break;
          case 1:   //launch bar switch in extend
            if (wowLeft == wowRight == wowNose == false){ // no wheight on wheels retract launch bar
              digitalWrite(LBAR_RET, LOW);
              launchBarMagState = LOW;  // mag is off
              break;
            }        
            if ((LBAR_SW_AUTORETRACT == true) && (rpmL >= 85 && rpmR >= 85)) {  //If Launch bar auto retract on throttle is true and both engines over 80% rpm turn off mag
              digitalWrite(LBAR_RET, LOW);
              launchBarMagState = LOW;  // mag is off
            }
            break;
        }
      }
    ```

## Variables & Constants

### Variables
* Use camelCase naming convention for variables.
* Avoid single letter variable names. Make them descriptive 
* Avoid variable names like `val` or `pin`. Be more descriptive, like `buttonState` or `switchPin` 
* Use the wiring/Processing-style variable types, e.g. boolean,char,byte,int,unsigned int,long,unsigned long,float,double,string,array,void when possible, rather than uint8_t, etc. The former are explained in the documentation, and less terse names. 
* Avoid numbering schemes that confuse the user, e.g.: 
    ```
    pin1 = 2
    pin2 = 3
    etc.
    ```

* If you need to renumber pins, consider using an array, like this: 
    `int myPins[] = { 2, 7, 6, 5, 4, 3 };`

    This allows you to refer to the new pin numbers using the array elements, like this: 
        `digitalWrite(myPins[1], HIGH);  // turns on pin 7`

    It also allows you to turn all the pins on or off in the sequence you want, like this: 
    ```
    for (int thisPin = 0; thisPin < 6; thisPin++) {
       digitalWrite(myPins[thisPin], HIGH);
       delay(500);
       digitalWrite(myPins[thisPin], LOW);
       delay(500);
    }
    ```
### Constants & Macros
* Use all caps for constants & macro values.
* If you want to define pin names and other quantities which won't change, use `#defines` or `const ints`. 
* `#defines` are preferred for simple pin assignments or other simple constant assignment. `const` declarations are acceptable where required. Avoid using `#defines` when it could be result in a hard-to-diagnose unfavorable outcome. [See this Arduino forum post for more information.](https://forum.arduino.cc/t/when-to-use-const-int-int-or-define/668071)
    ```
    // Define pins for DCS-BIOS per interconnect diagram.
    #define HOOKB_SW A0 ///< Hook Bypass Switch
    #define LBAR_SW A1 ///< Launch Bar Switch
    #define FLAPS_SW2 A2 ///< Flaps Switch Position 2
    #define FLAPS_SW1 A3 ///< Flaps Switch Position 1
    #define LBAR_RET 2    ///< Launch Bar Switch Mag
    #define HOOK_FIELD 3  ///< Hook Bypass Switch Mag
    #define LADG_SW 4 ///< Landing Taxi Light
    #define SJET_SW1 6 ///< Selector Jettison Position 1
    #define SJET_SW3 7 ///< Selector Jettison Position 3
    #define SJET_SW5 8 ///< Selector Jettison Position 8
    #define ASKID_SW 10 ///< Anti-Skid Switch
    #define SJET_SW2 14 ///< Selector Jettison Position 2
    #define SJET_PUSH 15 ///< Selector Jettison Push
    #define SJET_SW4 16 ///< Selector Jettison Position 
    ```

## Writing tutorials or instructions
* Write in the active voice. 
* Write clearly and conversationally, as if the person following your instructions were there in the room with you. 
* When giving instruction, write in the second person, so the reader understands that she's the one who'll be doing it. 
* Use short, simple, declarative sentences and commands rather than compound sentences. It's easier for the reader to digest one instruction at a time. 
* Give directions in no uncertain terms like so: 
    > "Next, you'll read the sensor..." 
    > "Make a variable called thisPin..." 
* Avoid phrases that add no information. Don't tell the reader that "You want to set the pins", just tell her "Set the pins." 
* Use pictures and schematics rather than just schematics alone. Many electronics hobbyists don't read schematics. 
* Check your assumptions. Does the reader understand all the concepts you've used in your tutorial? If not, explain them or link to another tutorial that does. 
* Explain things conceptually, so the reader has a big picture of what he's going to do. Then lay out instructions on how to use it step-by-step. 
* Whenever you use a technical term for the first time, define it. Have someone else check that you defined all new terms. There are probably one or two that you missed. 
* Be consistent with the terms you use. If you refer to a component or concept by a new name, make the relationship to the other name explicit. Don't use two terms interchangeably unless you tell the reader that they are interchangeable. 
* Don't use acronyms or abbreviations without spelling them out first. 
* Make your example do one thing well. Don't combine concepts or functions unless it's a tutorial about combining concepts. 

Adapted from: https://www.arduino.cc/en/Reference/StyleGuide
