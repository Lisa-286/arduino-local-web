#!/bin/bash
# Script semplice per caricare file su servizi cloud alternativi
# (non richiede OAuth complesso)

set -e

FILE="$1"
if [ -z "$FILE" ] || [ ! -f "$FILE" ]; then
    echo "Uso: $0 <file>"
    echo "Carica file su servizi cloud semplici"
    exit 1
fi

FILENAME=$(basename "$FILE")
SIZE=$(stat -c%s "$FILE" 2>/dev/null || stat -f%z "$FILE" 2>/dev/null)

echo "File: $FILENAME ($((SIZE/1024)) KB)"

# Opzione 1: 0x0.st (file hosting gratuito, max 512MB, 24h retention)
echo "=== Opzione 1: 0x0.st (gratuito, 24h) ==="
curl -# -F "file=@$FILE" https://0x0.st 2>/dev/null || echo "Fallito"

echo

# Opzione 2: transfer.sh (gratuito, 14 giorni)
echo "=== Opzione 2: transfer.sh (14 giorni) ==="
curl -# --upload-file "$FILE" https://transfer.sh/"$FILENAME" 2>/dev/null || echo "Fallito"

echo

# Opzione 3: file.io (gratuito, scaricabile una volta)
echo "=== Opzione 3: file.io (one-time download) ==="
curl -# -F "file=@$FILE" https://file.io 2>/dev/null | jq -r '.link' 2>/dev/null || echo "Fallito (installa jq per JSON parsing)"

echo

# Opzione 4: Creazione archivio GitHub Gist (testo)
if [[ "$FILENAME" =~ \.(ino|txt|md|py|sh|json)$ ]]; then
    echo "=== Opzione 4: GitHub Gist (per codice/testo) ==="
    # Richiede token GitHub in GITHUB_TOKEN env
    if [ -n "$GITHUB_TOKEN" ]; then
        DESCRIPTION="Arduino sketch: $FILENAME"
        CONTENT=$(cat "$FILE" | python3 -c "import sys, json; print(json.dumps(sys.stdin.read()))")
        JSON="{\"description\":\"$DESCRIPTION\",\"public\":true,\"files\":{\"$FILENAME\":{\"content\":$CONTENT}}}"
        curl -s -H "Authorization: token $GITHUB_TOKEN" \
             -H "Accept: application/vnd.github.v3+json" \
             -X POST https://api.github.com/gists \
             -d "$JSON" | jq -r '.html_url' 2>/dev/null || echo "Fallito"
    else
        echo "Imposta GITHUB_TOKEN per usare Gist"
    fi
fi

echo
echo "=== Istruzioni Google Drive Manuale ==="
echo "Per Google Drive:"
echo "1. Vai su https://drive.google.com"
echo "2. Clicca 'Nuovo' → 'Caricamento file'"
echo "3. Seleziona: $FILE"
echo "4. Dopo upload, clicca destro → 'Ottieni link'"
echo "5. Imposta 'Chiunque con il link può visualizzare'"
echo "6. Copia link e incollalo qui sotto"
