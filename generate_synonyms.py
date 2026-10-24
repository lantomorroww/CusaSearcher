import json
import re
import sys

def clean_title(title):
    """Убираем платформенные суффиксы и лишние пометки издания."""
    t = title.lower().strip()
    # убираем (PS5,PS4), (PS5), (PS4) и т.п.
    t = re.sub(r'\s*\(ps[0-9,\s]+\)', '', t, flags=re.IGNORECASE)
    # убираем технические пометки
    t = re.sub(r'\b(bundle|digital deluxe edition|deluxe edition|standard edition|'
               r'ultimate edition|complete edition|anniversary edition|'
               r'director\'?s cut|cross-gen bundle|digital deluxe|'
               r'enhanced edition|heritage edition|playstation plus)\b', '', t)
    # убираем двойные пробелы и висячие дефисы
    t = re.sub(r'\s+', ' ', t).strip(' -–:')
    return t

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 generate_synonyms.py games_data.json [synonyms.json]")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else "synonyms.json"

    with open(input_file, 'r', encoding='utf-8') as f:
        games = json.load(f)

    synonyms = {}

    for game in games:
        title = game.get("title", "")
        if not title:
            continue

        # Берём первый код (все регионы обычно имеют одинаковый код в базе)
        codes = list(game.get("regionCodes", {}).values())
        if not codes:
            continue
        code = codes[0]

        variants = set()

        # 1. Полное название в нижнем регистре
        full = title.lower().strip()
        variants.add(full)

        # 2. Название без суффикса платформы
        cleaned = clean_title(title)
        if cleaned:
            variants.add(cleaned)

        # 3. Вариант без "the " в начале
        if cleaned.startswith("the "):
            variants.add(cleaned[4:])

        # 4. Если есть двоеточие — добавим и часть до, и часть после
        if ':' in cleaned:
            before, after = cleaned.split(':', 1)
            if len(before.strip()) >= 3:
                variants.add(before.strip())
            if len(after.strip()) >= 3:
                variants.add(after.strip())

        # Добавляем в словарь, фильтруя пустые
        clean_variants = [v for v in variants if v and len(v) >= 2]

        if code not in synonyms:
            synonyms[code] = []
        for v in clean_variants:
            if v not in synonyms[code]:
                synonyms[code].append(v)

    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(synonyms, f, ensure_ascii=False, indent=4)

    print(f"Сгенерировано синонимов для {len(synonyms)} кодов -> {output_file}")

if __name__ == "__main__":
    main()
