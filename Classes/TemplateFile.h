//
//  TemplateFile.h
//  jumpproj
//
//  Created by Yanxing Wang on 2/6/16.
//
//

#ifndef TemplateFile_h
#define TemplateFile_h

static std::string sTemplateMap = R"({
  "spawnPosition": "700,200",
  "shadowGroup": [
     {
       "posx": 0,
       "width": 1,
       "lightType": "POINT",
       "lightDir": 50.2671,
       "lightPosition": "469.172,563.293",
       "lightMoving": false,
       "shadowDarkness": 0.098
     }
  ],
  "paletteFile": "palette/color_w1.json",
  "blocks": [
    {
      "id": 1,
      "size": "960,10",
      "position": "480,-5",
      "removable": false,
      "shadowEnable": false,
      "kind": "BLOCK"
    },
    {
      "id": 2,
      "size": "960,10",
      "position": "480,544.993",
      "removable": false,
      "shadowEnable": false,
      "kind": "BLOCK"
    },
    {
      "id": 3,
      "size": "10,559.993",
      "position": "-5,269.997",
      "removable": false,
      "shadowEnable": false,
      "kind": "BLOCK"
    },
    {
      "id": 4,
      "size": "10,559.993",
      "position": "965,269.997",
      "removable": false,
      "shadowEnable": false,
      "kind": "BLOCK"
    }
  ]
})";

#endif /* TemplateFile_h */
