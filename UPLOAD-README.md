# Caricamento File su Cloud

Script per caricare file (sketch Arduino, documentazione) su servizi cloud.

## Opzioni disponibili

### 1. Google Drive (OAuth2)
```bash
python3 upload-to-drive.py local-web.ino
```
**Prerequisiti**:
- File `credentials.json` da Google Cloud Console
- Prima esecuzione apre browser per autenticazione

**Per ottenere credentials.json**:
1. Vai a [Google Cloud Console](https://console.cloud.google.com/)
2. Crea nuovo progetto
3. Abilita Google Drive API
4. Crea credenziali OAuth 2.0 (Desktop app)
5. Scarica JSON come `credentials.json`

### 2. Servizi semplici (no autenticazione)
```bash
./upload-simple.sh local-web.ino
```
Carica su:
- **0x0.st** - Gratuito, 512MB, 24h retention
- **transfer.sh** - Gratuito, 14 giorni
- **file.io** - One-time download

### 3. GitHub Gist (per codice)
```bash
export GITHUB_TOKEN="tuo_token"
./upload-simple.sh local-web.ino
```
Crea Gist pubblico con il codice.

## File da caricare
- `local-web.ino` - Sketch Arduino hotspot standalone
- `README-LOCAL-WEB.md` - Documentazione
- Eventuali screenshot/demo

## Link rapidi
Dopo caricamento, condividi link con:
```
http://192.168.4.1 - Hotspot Arduino
[link cloud] - Sketch/download
```

## Note
- Google Drive richiede setup iniziale ma è permanente
- Servizi semplici sono immediati ma con limiti tempo
- Per distribuzione pubblica, preferire GitHub Gist o Drive
