# Calculator v3

Win32 GUI კალკულატორი, დაწერილი C++-ში. ინტერფეისი სრულად GDI+-ით იხატება, Windows 11-ის კალკულატორის სტილში.

![Calculator Screenshot](screenshot.png)

## ფუნქციები

- არითმეტიკა: `+` `−` `×` `÷`, ათწილადი რიცხვები
- `√` ფესვი, `!` ფაქტორიალი, `%` პროცენტი, `±` ნიშნის შეცვლა
- ბოლო ციფრის წაშლა (Backspace)
- ოპერაციების ჯაჭვი — `2 + 3 + 4 =`
- შეცდომების დამუშავება — ნულზე გაყოფა, უარყოფითის ფესვი, არამთელის ფაქტორიალი; მიზეზი დისპლეიზე ჩანს
- ისტორიის პანელი (უახლესი ზემოთ) და მისი გასუფთავება
- გამოთვლების ჩაწერა `%LOCALAPPDATA%\Calculator\history.txt`-ში (UTF-8, დროის შტამპით)
- კლავიატურის სრული მხარდაჭერა

## ინტერფეისი

- ერთი ფანჯარა — ღილაკები არ არის ცალკე Win32 კონტროლები, ყველაფერი ხელით იხატება
- GDI+: გლუვი კიდეები (anti-aliasing), გრადიენტი, ჩრდილი
- Hover და დაჭერის ეფექტი; დაჭერილი ღილაკიდან მაუსის გატანა მოქმედებას აუქმებს
- Double buffering — მოციმციმეობის გარეშე
- დისპლეის ფონტი ავტომატურად მცირდება გრძელ რიცხვებზე
- მუქი სათაურის ზოლი (DWM)
- სისტემური ხატულები (Segoe MDL2 Assets)
- საკუთარი ხატულა და ვერსიის ინფორმაცია `.exe`-ში

## გამოყენებული ტექნოლოგიები

- **C++**
- **Win32 API** — User32
- **GDI+** — ხატვა
- **DWM** — მუქი სათაური
- **Inno Setup** — ინსტალერი
- Visual Studio 2022

## ჩამოტვირთვა

მზა ინსტალერი — [Releases](https://github.com/levanikotorashvili20-spec/calculator-v3/releases) → `CalculatorSetup-1.0.0.exe`

> Windows შეიძლება გამოიტანოს შეტყობინება "Windows protected your PC" — პროგრამას ციფრული ხელმოწერა არ აქვს. გასაგრძელებლად: **More info → Run anyway**.

## პროექტის აგება (Build)

1. გახსენი `calculator v3.slnx` Visual Studio-ში
2. Configuration: **Release**, Platform: **x86**
3. `Build → Build Solution` (`Ctrl+Shift+B`)
4. გაშვებადი ფაილი: `Release/calculator v3.exe`

`gdiplus.lib` და `dwmapi.lib` კოდშივეა მიერთებული (`#pragma comment`), C++ runtime კი სტატიკურად (`/MT`) — `.exe` დამატებითი DLL-ების გარეშე ეშვება.

### ინსტალერის აგება

1. დააყენე [Inno Setup](https://jrsoftware.org/isdl.php)
2. ააგე Release ვერსია (იხ. ზემოთ)
3. გახსენი `installer/setup.iss` Inno Setup-ში → `Build → Compile`
4. ინსტალერი: `installer/output/CalculatorSetup-1.0.0.exe`

## კლავიატურა

| კლავიში | მოქმედება |
|---|---|
| `0`-`9`, `.` | რიცხვი |
| `+` `-` `*` `/` | ოპერაციები |
| `Enter` / `=` | შედეგი |
| `Backspace` | ბოლო ციფრის წაშლა |
| `Esc` / `Delete` / `C` | გასუფთავება |
| `@` | ფესვი |
| `!` | ფაქტორიალი |
| `%` | პროცენტი |
| `F9` | ნიშნის შეცვლა |
| `F1` | დახმარება |

## პროექტის სტრუქტურა

| ფოლდერი | შიგთავსი |
|---|---|
| `src/` | წყარო კოდი |
| `resources/` | ხატულა და ვერსიის ინფორმაცია (`app.rc`) |
| `installer/` | Inno Setup სკრიპტი |

კოდის დეტალური აღწერა — [ARCHITECTURE.md](ARCHITECTURE.md).

## ვერსიები

ერთი და იგივე კალკულატორი, სამ სხვადასხვა ენაზე:

- [Calculator](https://github.com/levanikotorashvili20-spec/Calculator) — x86 Assembly
- [calculator-v2](https://github.com/levanikotorashvili20-spec/calculator-v2) — C
- **calculator-v3** — C++ (ეს პროექტი)

## ავტორი

Levan Kotorashvili
GitHub: [@levanikotorashvili20-spec](https://github.com/levanikotorashvili20-spec)

## ლიცენზია

MIT — იხილეთ [LICENSE](LICENSE).