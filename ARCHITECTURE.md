\# Architecture



\## ფაილების სია



| ფაილი | დანიშნულება |

|---|---|

| `colors.h` | ფერების პალიტრა |

| `calc.h` | ღილაკების ID-ები, ოპერაციები, განლაგების ზომები, `Button`, `HistoryEntry` |

| `History.h` / `.cpp` | ისტორია მეხსიერებაში + `history.txt`-ში ჩაწერა |

| `Calculator.h` / `.cpp` | მთელი ლოგიკა — Windows-ზე არ არის დამოკიდებული |

| `Layout.h` / `.cpp` | ღილაკების ბადე, `HitTest` |

| `Renderer.h` / `.cpp` | GDI+ ხატვა |

| `main.cpp` | `wWinMain`, `WndProc`, მაუსი, კლავიატურა, GDI+-ის სიცოცხლის ციკლი |



\## ფენები



```

main.cpp        ფანჯარა, მაუსი, კლავიატურა

&#x20;  │

&#x20;  ├── Renderer     ხატვა (GDI+)  ──►  Layout   სად დგას ღილაკები

&#x20;  │

&#x20;  └── Calculator   ლოგიკა        ──►  History  მეხსიერება + ფაილი

```



`Calculator` და `History` Windows-ის ფანჯრების შესახებ არაფერი იციან —

მხოლოდ ID-ს იღებენ და ტექსტს აბრუნებენ. ლოგიკა ინტერფეისისგან

სრულად გამიჯნულია.



\## Build მოდელი



Unity build: `main.cpp` დანარჩენ `.cpp` ფაილებს `#include`-ით ჩასვამს.

`History.cpp`, `Calculator.cpp`, `Layout.cpp`, `Renderer.cpp` —

\*\*Excluded From Build: Yes\*\*.



\## შეყვანის ნაკადი



```

მაუსი:     WM\_LBUTTONDOWN → ღილაკი "ჩადის" (SetCapture)

&#x20;          WM\_LBUTTONUP   → იმავე ღილაკზეა? → Activate(id)



კლავიატურა: WM\_CHAR    → CharToId  → Activate(id)

&#x20;           WM\_KEYDOWN → KeyToId   → Activate(id)   (F1, F9, Delete)



Activate(id):

&#x20;  ID\_HELP          → ShowHelp

&#x20;  ID\_CLEAR\_HISTORY → Calculator::ClearHistory

&#x20;  სხვა             → Calculator::HandleInput

&#x20;       ↓

&#x20;  InvalidateRect → WM\_PAINT → Renderer::Paint

```



\## ხატვის ნაკადი



```

Renderer::Paint

&#x20;  1. Bitmap მეხსიერებაში (double buffer)

&#x20;  2. ფონი

&#x20;  3. DrawDisplay   — გამოსახულება + რიცხვი (ფონტი ავტომატურად ეტევა)

&#x20;  4. DrawButton    — ჩრდილი → გრადიენტი → კონტური → ტექსტი

&#x20;  5. DrawHistory   — სათაური, ჩანაწერები, გასუფთავების ღილაკი

&#x20;  6. DrawImage     — მზა სურათი ეკრანზე ერთი ოპერაციით

```



\## Calculator-ის მდგომარეობა



| ცვლადი | დანიშნულება |

|---|---|

| `m\_input` | ეკრანზე დიდად (სტრიქონად — `.` და Backspace-ისთვის) |

| `m\_expression` | ეკრანზე წვრილად: `15 + 12 =` ან შეცდომის მიზეზი |

| `m\_stored` | პირველი რიცხვი |

| `m\_op` | არჩეული ოპერაცია |

| `m\_newNumber` | შემდეგი ციფრი ახალ რიცხვს იწყებს |

| `m\_hasOperand` | ოპერატორის შემდეგ მეორე რიცხვი არსებობს (ჯაჭვისთვის) |

| `m\_error` | შეცდომის მდგომარეობა |



\## GDI+-ის სიცოცხლის ციკლი



```

wWinMain:

&#x20;  GdiplusSession gdiplus;   ← GdiplusStartup

&#x20;  Renderer       renderer;  ← ფონტები იქმნება

&#x20;  ...

&#x20;  }                          ← ჯერ renderer ნადგურდება, მერე GdiplusShutdown

```



C++ ობიექტებს შექმნის საპირისპირო რიგით ანადგურებს — ეს გარანტიას

იძლევა, რომ GDI+ ობიექტები GDI+-ის გამორთვამდე წაიშლება.



\## ღილაკების ID-ები



| ID | ღილაკი |

|---|---|

| 100-109 | ციფრები 0-9 |

| 110 | `.` |

| 111 | C |

| 112 | `=` |

| 113-116 | `÷` `×` `−` `+` |

| 117 | `?` |

| 118 | `%` |

| 119 | `√` |

| 120 | `!` |

| 121 | `±` |

| 122 | Backspace |

| 123 | ისტორიის გასუფთავება |

