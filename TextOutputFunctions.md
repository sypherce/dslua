# Text Output Functions #
> These functions are used to output text to the screen. To initialize a screen to display text, see the `Screen.LoadTextBG()` and `Screen.Initialize()` functions.

  * `TextBackground:SetColor()`
  * `TextBackground:PrintXY()`
  * `TextBackground:Clear()`

## `TextBackground:SetColor(color)` ##
> Set the text color for that background. All previously printed text on the background will stay the same color, only the text that is printed after the `SetColor` function is affected.

> _**color**_ can be a number from 0 to 9. 0 = white, 1 = red, 2 = green, 3 = blue, 4 = purple, 5 = cyan, 6 = yellow, 7 = light gray, 8 = dark gray, 9 = black.

## `TextBackground:PrintXY(x, y, text)` ##
> Print text to the screen.

> _**x**_ can be a number from 0 to 31. It controls where the text will start printing to, horizontally.

> _**y**_ can be a number from 0 to 23. It controls where the text will start printing to, vertically.

> _**text**_ is the string that will be printed.

## `TextBackground:Clear()` ##
> Clear all text on that background.

[Back to index](DSLuaDocumentation.md)