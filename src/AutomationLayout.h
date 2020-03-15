
#define AUTOSTYLEBUTTON 0
#define AUTOSTYLETOPBAR 1
#define AUTOSTYLESUBBAR 2
#define AUTOSTYLEOPTIONS 3

const AUTOSTYLE auto_styles[] = {
  { //AUTOSTYLEBUTTON
    .fill = ILI9341_DARKGREY,
    .border = ILI9341_BLACK,
    .color = ILI9341_WHITE,
    .alignment = AUTO_ALIGNCENTER,
    .font = false
  },
  { //AUTOSTYLETOPBAR
    .fill = ILI9341_BLUE,
    .border = ILI9341_WHITE,
    .color = ILI9341_YELLOW,
    .alignment = AUTO_ALIGNCENTER,
    .font = true
  },
  { //AUTOSTYLESUBBAR
    .fill = ILI9341_BLUE,
    .border = ILI9341_WHITE,
    .color = ILI9341_YELLOW,
    .alignment = AUTO_ALIGNCENTER,
    .font = false
  },
  { //AUTOSTYLEOPTIONS
    .fill = ILI9341_WHITE,
    .border = ILI9341_WHITE,
    .color = ILI9341_BLACK,
    .alignment = AUTO_ALIGNLEFT,
    .font = false
  }
};
