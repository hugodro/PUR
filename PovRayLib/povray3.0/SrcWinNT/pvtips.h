

char *tips [] =
{
  "Many frequently asked questions refer to things already in the documentation "
  "and even these tip dialogs. Be sure to read the help file (press F1 after "
  "dismissing this dialog) and to read all of these tips before asking a question.",

  "By default, if you double-click on a file using the Windows 95 Explorer "
  "while POV-Ray is already loaded, a new copy of POV-Ray will be started. "
  "You can change this to start the file in the already-loaded copy of POV-Ray "
  "by using 'Appearance|Keep Single Instance' (when '&Appearance|&Expert Menus' "
  "are on).",

  "You can open a file for editing or rendering from the Windows 95 Explorer by "
  "using the right mouse button when you click on the file name. This feature even "
  "works with POV-Ray's own internal file browse dialogs. For example, dismiss this "
  "tip dialog, press ALT-S, choose a .POV or .INI file, press the right mouse button "
  "and see what happens. (Windows 95 users only.)",

  "By default, POV-Ray's rendering speed under Windows will be slower than DOS "
  "because it tries to be 'nice' to other programs by not hogging the CPU. If "
  "time is an issue (or you want to do a benchmark), you need to set the priority "
  "of the renderer from the '&Render|Render &Priority' menu. Windows 3.1 users will "
  "find this entry called 'Niceness' instead.",

  "You can select a plain background, or a different bitmap for the tiled "
  "background, from the &Appearance menu. A shortcut for the bitmap file browse "
  "dialog box is ALT-P. POV-Ray for Windows comes with a number of tileable "
  "backgrounds for you to use.",

  "If the special window drawing effects (like the extra button) don't look good "
  "on your system, or if you just don't like them, you can turn them off from "
  "the &Appearance menu.",

  "You can access a number of commonly-used functions by pressing the right "
  "mouse button while over a window. You will get a different pop-up menu for "
  "both the main window and the render window.",

  "To close the small 'message window' that is displayed below the editor "
  "during rendering (or after an error), right-click on the EDITOR window and "
  "select 'Hide Messages'.",

  "You can cause POV-Ray to output all image files to a single directory by "
  "specifying it using Output_File_Name. E.g., to output all image files to "
  "the directory c:\\images, use Output_File_Name=c:\\images\\ in either an INI "
  "file, or on the command-line. Unless you explicitly specify an output file, "
  "the output file will be automatically placed in that directory.",

  "You can change the default colour and font for the message display from the "
  "&Appearance menu.",

  "If you are using POV-Ray's own editor, POV-Edit, you can press Ctrl-F1 when "
  "the cursor is on a word to do a help-file lookup for that word.",

  "By default, POV-Ray for Windows will allow you to start multiple copies "
  "of itself. This seems to be the behaviour that most people want. However, "
  "it is not always desirable. To change this, select 'Appearance|Keep Single Instance' "
  "(when '&Appearance|&Expert Menus' are on.)",

  "That while the default output file type is uncompressed TGA, POV-Ray for Windows "
  "can also write compressed TGA (+fc), BMP (+fs), PNG (+fn) and PPM (+fp).",

  "You can drag'n'drop .BMP files onto POV-Ray's main window, and it will tile "
  "and display them in the same way as if you had selected "
  "'&Appearance|Tiling &Options|&Browse'. You can Drag'N'Drop .POV and .INI files "
  "too, and POV will either render them or edit them (depending on the setting of "
  "the menu '&Misc|&Drag'N'Drop Destination'.",

  "You can supply command-line parameters to POV-Ray for Windows in exactly the "
  "same way as for the DOS version. If any parameters are on the command-line "
  "when POV-Ray starts, it will fire up the rendering engine straight away. "
  "This allows you to set up icons for commonly-used tasks you may perform. "
  "Read the help file section on command-line parameters for more information.",

  "That it is also legal to supply INI file parameters on the command line ? "
  "For example, instead of \"+ifilename.pov\" you can say \"Input_File_Name="
  "filename.pov\". This is actually essential if you are using long filenames "
  "that contain spaces, as the conventional \"+i\" form does not work with those "
  "types of filenames.",

  "That you can move the main POV-Ray for Windows window by clicking on and dragging "
  "the status bar at the bottom of the window. This works in exactly the same way as "
  "clicking on the caption bar does with an ordinary window. Double-clicking will "
  "also maximize the window, as you'd expect.",

  "That you can customise the &Tools menu ? You can add your own entries (up to 32 "
  "in fact) that can execute any external program. POV-Ray for Windows allows you to "
  "specify parameters, such as '%r', which expands to the last rendered file.",

  NULL
} ;



