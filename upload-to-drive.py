#!/usr/bin/env python3
"""
Script per caricare file su Google Drive.
Richiede autenticazione OAuth2 e file credentials.json.
"""

import os
import sys
from pathlib import Path
import json
import pickle
from googleapiclient.discovery import build
from googleapiclient.http import MediaFileUpload
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request

# Scopi API Drive
SCOPES = ['https://www.googleapis.com/auth/drive.file']

def get_drive_service():
    """Autentica e restituisce servizio Drive."""
    creds = None
    token_file = 'token.pickle'
    
    # Carica token esistente
    if os.path.exists(token_file):
        with open(token_file, 'rb') as token:
            creds = pickle.load(token)
    
    # Se non valide, richiedi nuovo token
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            # Cerca credentials.json
            creds_files = ['credentials.json', 'client_secret.json']
            creds_path = None
            for f in creds_files:
                if os.path.exists(f):
                    creds_path = f
                    break
            
            if not creds_path:
                print("File credentials.json non trovato.")
                print("Scaricalo da Google Cloud Console:")
                print("1. Vai a https://console.cloud.google.com/")
                print("2. Crea progetto → API & Services → Credentials")
                print("3. Crea OAuth 2.0 Client ID (Desktop app)")
                print("4. Scarica JSON come 'credentials.json'")
                sys.exit(1)
            
            flow = InstalledAppFlow.from_client_secrets_file(creds_path, SCOPES)
            creds = flow.run_local_server(port=0)
        
        # Salva token per prossime esecuzioni
        with open(token_file, 'wb') as token:
            pickle.dump(creds, token)
    
    return build('drive', 'v3', credentials=creds)

def upload_file(service, file_path, folder_id=None):
    """Carica file su Drive."""
    file_name = os.path.basename(file_path)
    
    file_metadata = {
        'name': file_name,
        'parents': [folder_id] if folder_id else []
    }
    
    media = MediaFileUpload(file_path, resumable=True)
    
    print(f"Caricamento {file_name}...")
    file = service.files().create(
        body=file_metadata,
        media_body=media,
        fields='id, name, webViewLink'
    ).execute()
    
    print(f"✅ Caricato: {file.get('name')}")
    print(f"🔗 Link: {file.get('webViewLink')}")
    return file

def list_folders(service):
    """Lista cartelle Drive."""
    results = service.files().list(
        q="mimeType='application/vnd.google-apps.folder'",
        pageSize=10,
        fields="files(id, name)"
    ).execute()
    
    folders = results.get('files', [])
    if folders:
        print("Cartelle disponibili:")
        for f in folders:
            print(f"  {f['name']} (ID: {f['id']})")
    else:
        print("Nessuna cartella trovata.")
    return folders

def main():
    if len(sys.argv) < 2:
        print("Uso: python3 upload-to-drive.py <file1> [file2 ...]")
        print("Opzioni:")
        print("  --folder-id <ID>   Carica in cartella specifica")
        print("  --list-folders     Lista cartelle Drive")
        sys.exit(1)
    
    # Parse argomenti
    files = []
    folder_id = None
    list_folders_flag = False
    
    i = 1
    while i < len(sys.argv):
        arg = sys.argv[i]
        if arg == '--folder-id' and i+1 < len(sys.argv):
            folder_id = sys.argv[i+1]
            i += 2
        elif arg == '--list-folders':
            list_folders_flag = True
            i += 1
        elif arg.startswith('--'):
            print(f"Opzione sconosciuta: {arg}")
            sys.exit(1)
        else:
            if os.path.exists(arg):
                files.append(arg)
            else:
                print(f"File non trovato: {arg}")
            i += 1
    
    # Autentica
    try:
        service = get_drive_service()
    except Exception as e:
        print(f"Errore autenticazione: {e}")
        sys.exit(1)
    
    # Lista cartelle se richiesto
    if list_folders_flag:
        list_folders(service)
        return
    
    # Carica file
    if not files:
        print("Nessun file specificato.")
        sys.exit(1)
    
    for file_path in files:
        if not os.path.exists(file_path):
            print(f"File non trovato: {file_path}")
            continue
        
        try:
            upload_file(service, file_path, folder_id)
        except Exception as e:
            print(f"❌ Errore caricamento {file_path}: {e}")

if __name__ == '__main__':
    main()
