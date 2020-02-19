# Dark Souls III Item Randomiser (C++)

Made the mod open source, I had to rewrite it because even I didn't know what I was thinking back when it was made in 2018 - It might have better performance actually.

## I want to make my own list

To make your own list, you'll need to just swap out the "ItemList.txt" file with a list of your own. You can do this without needing to edit the source code so long as it's a list of 1600 items.

If you want to customise it further, with a custom length list, you'll need to edit the source code. You can just change the `#define MAX_LIST_ITEMS 1600` to a number of your choosing, then it should work. 

If you want to make your own complex list, with exceptions and rules (e.g not infusing/upgrading certain weapons) you'll need to modify the hardcoded lists in the mod. You can find them in "ItemRandomiser\ItemRandomiser.cpp". I know my code is pretty horrible to look at though, so it might just be a case of trial and error until it works. I'll upload the AoBRandomiser.exe source later.

To test things out, you'll probably want the debug mode which just pops up a console with information about what items are being randomised and when, making it a lot less of a headache to debug if it explodes for seemingly no reason. To activate this, just remove the `//` from the `//#define DEBUG` in Core.h at line 13.

## Can I re-distribute it if I make modifications?

If you've made something interesting and you want to share it with others, go ahead. I'd appreciate a credit/link back to this page though.

## Libraries used

MinHook: https://github.com/TsudaKageyu/minhook
INIReader: https://github.com/benhoyt/inih
DLL_Wrapper_Generator (for the dinput8 wrapper): https://github.com/SeanPesce/DLL_Wrapper_Generator
