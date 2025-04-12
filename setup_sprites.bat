@echo off
echo Setting up sprite assets...

REM Create directories if they don't exist
if not exist "assets\sprites" mkdir "assets\sprites"

REM Download placeholder images
echo Downloading placeholder images...

REM Background
curl -L "https://raw.githubusercontent.com/placeholder-images/placeholder-images/master/800x600.png" -o "assets\sprites\background.png"

REM Button states
curl -L "https://raw.githubusercontent.com/placeholder-images/placeholder-images/master/100x30.png" -o "assets\sprites\button_normal.png"
curl -L "https://raw.githubusercontent.com/placeholder-images/placeholder-images/master/100x30.png" -o "assets\sprites\button_hover.png"
curl -L "https://raw.githubusercontent.com/placeholder-images/placeholder-images/master/100x30.png" -o "assets\sprites\button_pressed.png"

REM Task dialog
curl -L "https://raw.githubusercontent.com/placeholder-images/placeholder-images/master/400x400.png" -o "assets\sprites\task_dialog.png"

REM Checkboxes
curl -L "https://raw.githubusercontent.com/placeholder-images/placeholder-images/master/30x30.png" -o "assets\sprites\checkbox_unchecked.png"
curl -L "https://raw.githubusercontent.com/placeholder-images/placeholder-images/master/30x30.png" -o "assets\sprites\checkbox_checked.png"

REM Icons
curl -L "https://raw.githubusercontent.com/placeholder-images/placeholder-images/master/32x32.png" -o "assets\sprites\icon_habit.png"
curl -L "https://raw.githubusercontent.com/placeholder-images/placeholder-images/master/32x32.png" -o "assets\sprites\icon_daily.png"
curl -L "https://raw.githubusercontent.com/placeholder-images/placeholder-images/master/32x32.png" -o "assets\sprites\icon_todo.png"
curl -L "https://raw.githubusercontent.com/placeholder-images/placeholder-images/master/32x32.png" -o "assets\sprites\icon_difficulty.png"
curl -L "https://raw.githubusercontent.com/placeholder-images/placeholder-images/master/32x32.png" -o "assets\sprites\icon_edit.png"
curl -L "https://raw.githubusercontent.com/placeholder-images/placeholder-images/master/32x32.png" -o "assets\sprites\icon_delete.png"

echo Sprite assets setup complete!
echo You can now replace these placeholder images with your own custom sprites. 