# AC Anticheat Comportamental (ac.inc)

Anti-cheat comportamental leve, server-side, para SA-MP/open.mp, focado em performance, baixo falso positivo e integração simples.

## Créditos
- Autor: RickZin
- Discord: rickzin021
- GitHub: https://github.com/rickcosta-dev
- Portfólio: https://rickzindev.vercel.app/

## Recursos
- Coleta por eventos (teclas, movimento, comandos) separada da decisão
- Três categorias de score: macro, bot e script
- Decaimento não-linear automático via timer
- Correlação de eventos em janela curta para reforçar evidência
- Logs agregados com colapso de eventos repetidos
- Compatibilidade com constantes e thresholds legados

## Requisitos
- SA-MP 0.3.7 R2-2+ ou open.mp
- Inclua `ac.inc` na sua GM

## Instalação
1. Copie `ac.inc` para a pasta de includes do seu projeto.
2. Adicione `#include <ac>` na sua gamemode.
3. Crie um timer global chamando `AC_Process()` a cada 500 ms.

## Integração rápida
```pawn
// Timer global
forward AC_Tick();
public AC_Tick() { AC_Process(); return 1; }

public OnGameModeInit()
{
    SetTimer("AC_Tick", 500, true);
    return 1;
}

public OnPlayerConnect(playerid)
{
    AC_InitPlayer(playerid);
    return 1;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    AC_UpdateKeys(playerid, newkeys, oldkeys);
    if (AC_ShouldBan(playerid)) Ban(playerid);
    else if (AC_ShouldKick(playerid)) Kick(playerid);
    return 1;
}

public OnPlayerUpdate(playerid)
{
    AC_UpdateMovement(playerid);
    return 1;
}

// Após executar cada comando do jogador:
// AC_UpdateCommand(playerid);
```

## API Pública
- Coleta
  - `AC_InitPlayer(playerid)`
  - `AC_OnKeys(playerid, newkeys, oldkeys)` / `AC_UpdateKeys(...)`
  - `AC_OnMovement(playerid)` / `AC_UpdateMovement(...)`
  - `AC_OnCommand(playerid)` / `AC_UpdateCommand(...)`
- Processamento
  - `AC_Process()` — dispara decaimento e verificações contínuas
- Score
  - `AC_GetScoreCat(playerid, category)` — categoria: `SCORE_MACRO`, `SCORE_BOT`, `SCORE_SCRIPT`
  - `AC_GetScore(playerid)` — soma das categorias
- Decisão
  - `bool:AC_ShouldKick(playerid)`
  - `bool:AC_ShouldBan(playerid)`
  - `bool:AC_ShouldPunish(playerid)` — verdadeiro se deve kickar ou banir
- Logs
  - `AC_LogDump(playerid)` — imprime resumo das evidências acumuladas

## Configuração
- Principais definições:
  - `AC_PROCESS_INTERVAL_MS = 500` — intervalo do processamento
  - `AC_GRACE_SECONDS = 20` — período de graça sem punição
  - `AC_CORRELATION_WINDOW = 1500` — janela para correlação de eventos
  - `AC_SCORE_MACRO_LEVE = 10`, `AC_SCORE_MACRO_EVIDENTE = 25`, `AC_SCORE_SCRIPT_CLARO = 40`
  - Thresholds legados: `AC_FLAG_SUSPEITO = 40`, `AC_FLAG_KICK = 70`, `AC_FLAG_BAN = 100`
- Logs:
  - `AC_LOG_ENABLED`, `AC_LOG_SIZE`, `AC_LOG_COLLAPSE_WINDOW`, `AC_LOG_IMMEDIATE`

## Como funciona
- Teclas: alternâncias múltiplas num único tick acumulam score de macro
- Turn+Fire: virar muito e atirar logo após movimento marca script
- Comandos: intervalos constantes repetidos acumulam score de macro
- Reação: tempo entre input e comando abaixo do humano marca bot
- Movimento perfeito: passos idênticos por muitos ticks marcam script
- Eventos correlacionados em janela curta pontuam em dobro
- Decaimento não-linear reduz score ao longo do tempo

## Compatibilidade
- Mapeia teclas ausentes: `KEY_AIM`, `KEY_LEFT/RIGHT/UP/DOWN`
- Expõe thresholds legados para GMs que usam score total diretamente
- Silencia aviso `SAMP_INCLUDES_VERSION` quando presente

## Boas práticas
- Use `AC_ShouldKick/AC_ShouldBan` para punir; respeitam período de graça e categorias
- Não chame `AC_Process` por player; mantenha um único timer global
- Sempre chame `AC_UpdateCommand` após executar comandos de jogador

## Licença
Sem licença definida no include. Defina a licença ao publicar no GitHub (ex.: MIT).
