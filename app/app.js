const express = require('express');
const os = require('os');

const app = express();
const PORT = process.env.PORT || 3000;

// Middleware pour logger les requêtes
app.use((req, res, next) => {
    const timestamp = new Date().toISOString();
    console.log(`[${timestamp}] ${req.method} ${req.url} - IP: ${req.ip}`);
    next();
});

// Route principale
app.get('/', (req, res) => {
    res.json({
        message: '🚀 Bienvenue sur l\'API Whanos Example!',
        status: 'running',
        timestamp: new Date().toISOString(),
        pod: {
            hostname: os.hostname(),
            platform: os.platform(),
            arch: os.arch()
        }
    });
});

// Health check endpoint
app.get('/health', (req, res) => {
    const uptime = process.uptime();
    res.json({
        status: 'healthy',
        uptime: `${Math.floor(uptime)}s`,
        memory: {
            used: `${Math.round(process.memoryUsage().heapUsed / 1024 / 1024)}MB`,
            total: `${Math.round(process.memoryUsage().heapTotal / 1024 / 1024)}MB`
        }
    });
});

// Informations sur le pod
app.get('/info', (req, res) => {
    res.json({
        pod: {
            hostname: os.hostname(),
            platform: os.platform(),
            arch: os.arch(),
            cpus: os.cpus().length,
            totalMemory: `${Math.round(os.totalmem() / 1024 / 1024)}MB`,
            freeMemory: `${Math.round(os.freemem() / 1024 / 1024)}MB`,
            uptime: `${Math.floor(os.uptime())}s`
        },
        process: {
            pid: process.pid,
            nodeVersion: process.version,
            uptime: `${Math.floor(process.uptime())}s`,
            memoryUsage: {
                heapUsed: `${Math.round(process.memoryUsage().heapUsed / 1024 / 1024)}MB`,
                heapTotal: `${Math.round(process.memoryUsage().heapTotal / 1024 / 1024)}MB`,
                rss: `${Math.round(process.memoryUsage().rss / 1024 / 1024)}MB`
            }
        },
        environment: {
            nodeEnv: process.env.NODE_ENV || 'development',
            port: PORT
        }
    });
});

// Variables d'environnement (debug)
app.get('/env', (req, res) => {
    // Filtrer les variables sensibles
    const safeEnv = Object.keys(process.env)
        .filter(key => !key.toLowerCase().includes('secret') && 
                      !key.toLowerCase().includes('password') &&
                      !key.toLowerCase().includes('token'))
        .reduce((obj, key) => {
            obj[key] = process.env[key];
            return obj;
        }, {});
    
    res.json(safeEnv);
});

// Route pour tester la charge
app.get('/load', (req, res) => {
    const start = Date.now();
    // Simule un calcul lourd
    let sum = 0;
    for (let i = 0; i < 10000000; i++) {
        sum += Math.sqrt(i);
    }
    const duration = Date.now() - start;
    
    res.json({
        message: 'Test de charge terminé',
        duration: `${duration}ms`,
        result: sum,
        pod: os.hostname()
    });
});

// Route 404
app.use((req, res) => {
    res.status(404).json({
        error: 'Route non trouvée',
        path: req.url,
        availableRoutes: [
            'GET /',
            'GET /health',
            'GET /info',
            'GET /env',
            'GET /load'
        ]
    });
});

// Gestion des erreurs
app.use((err, req, res, next) => {
    console.error('Erreur:', err);
    res.status(500).json({
        error: 'Erreur interne du serveur',
        message: err.message,
        pod: os.hostname()
    });
});

// Démarrage du serveur
app.listen(PORT, '0.0.0.0', () => {
    console.log('========================================');
    console.log(`🚀 Serveur démarré avec succès!`);
    console.log(`📍 Port: ${PORT}`);
    console.log(`🖥️  Hostname: ${os.hostname()}`);
    console.log(`⏰ Démarré à: ${new Date().toISOString()}`);
    console.log('========================================');
});

// Graceful shutdown
process.on('SIGTERM', () => {
    console.log('SIGTERM reçu, arrêt gracieux...');
    process.exit(0);
});

process.on('SIGINT', () => {
    console.log('SIGINT reçu, arrêt gracieux...');
    process.exit(0);
});
