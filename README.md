# Exemple d'Application Whanos - Node.js API

Cette application est un exemple complet montrant comment utiliser Whanos pour le déploiement automatique sur Kubernetes.

## Fonctionnalités démontrées

- ✅ Détection automatique du langage (JavaScript/Node.js)
- ✅ Build automatique de l'image Docker
- ✅ Déploiement automatique sur Kubernetes
- ✅ Exposition des ports via NodePort
- ✅ Configuration des ressources
- ✅ Haute disponibilité avec 3 réplicas

## Structure du projet

```
example-nodejs-api/
├── app/
│   └── app.js           # Code de l'application
├── package.json         # Dépendances Node.js (détection Whanos)
├── whanos.yml          # Configuration de déploiement
└── README.md           # Documentation
```

## Configuration Whanos (whanos.yml)

```yaml
deployment:
  replicas: 3
  resources:
    limits:
      cpu: "500m"
      memory: "512Mi"
    requests:
      cpu: "250m"
      memory: "256Mi"
  ports:
    - 3000
```

## Test en local

```bash
# Installer les dépendances
npm install

# Lancer l'application
npm start

# Tester
curl http://localhost:3000
curl http://localhost:3000/health
```

## Déploiement avec Whanos

1. Poussez ce projet sur un repository Git
2. Dans Jenkins, lancez le job `link-project`
3. Paramètres :
   - **DISPLAY_NAME**: `nodejs-api-example`
   - **GIT_REPOSITORY**: URL de votre repository

## Accès après déploiement

```bash
# Vérifier le déploiement
kubectl get pods -l app=nodejs-api-example
kubectl get service nodejs-api-example

# Récupérer le NodePort
NODE_PORT=$(kubectl get service nodejs-api-example -o jsonpath='{.spec.ports[0].nodePort}')
echo "Application accessible sur: http://<node-ip>:$NODE_PORT"

# Tester l'application
curl http://<node-ip>:$NODE_PORT/
curl http://<node-ip>:$NODE_PORT/health
```

## Endpoints disponibles

- `GET /` - Page d'accueil
- `GET /health` - Health check
- `GET /info` - Informations sur le pod
- `GET /env` - Variables d'environnement (debug)

## Surveillance

```bash
# Logs en temps réel
kubectl logs -f -l app=nodejs-api-example

# Métriques des pods
kubectl top pods -l app=nodejs-api-example

# Événements
kubectl get events --sort-by=.metadata.creationTimestamp | grep nodejs-api-example
```

## Mise à jour

Pour mettre à jour l'application :
1. Modifiez le code
2. Poussez sur Git
3. Jenkins détecte le changement et redéploie automatiquement

## Rollback

Si quelque chose ne va pas :

```bash
# Voir l'historique
helm history nodejs-api-example

# Rollback vers la version précédente
helm rollback nodejs-api-example
```
