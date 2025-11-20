webserv – École 42

Un serveur HTTP/1.1 écrit en C++98, capable de gérer plusieurs connexions simultanées non bloquantes, d’interpréter un fichier de configuration et de servir des contenus statiques et dynamiques (CGI).
Projet inspiré des comportements réels de serveurs tels que Nginx.

Le but de ce projet est de comprendre le fonctionnement interne d’un serveur web :
gestion des sockets, multiplexage, parsing HTTP, gestion d’erreurs, et maintenance d’un serveur robuste.
Aucune bibliothèque externe de réseau n’est utilisée, tout est codé from scratch selon la norme C++98.

🚀 Fonctionnalités
Core

Serveur HTTP/1.1 complet, non-bloquant

Multiplexage via poll

Gestion de plusieurs serveurs virtuels (server blocks)

Parsing d’un fichier de configuration personnalisé

Méthodes supportées :

GET

POST

DELETE

Gestion des codes de statut HTTP

Gestion des en-têtes : Host, Content-Length, Content-Type, Connection, etc.

Routing avancé avec location

Gestion des fichiers statiques

Pages d’erreur personnalisées

Timeouts connexion / requête

Upload simple de fichiers via POST

✔️ Auto-index

Affichage automatique du contenu d’un répertoire lorsque autoindex on est activé.

✔️ Support CGI

Interprétation de scripts :

Python (.py)

PHP (optionnel)

Variables d’environnement respectant la spec CGI

Timeout CGI + gestion des erreurs

✔️ Cookies & Sessions simples

Envoi / lecture de cookies

Exemple d’un mini gestionnaire de session basé sur un cookie unique

✔️ Support upload avancé

Formulaire multipart

Sauvegarde dans un dossier défini via upload_path
