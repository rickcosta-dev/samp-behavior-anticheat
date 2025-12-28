# BehaviorID Anti-Cheat — Documentation

## Créditos
- Autor: RickZin
- Discord: rickzin021
- GitHub: https://github.com/rickcosta-dev
- Portfólio: https://rickzindev.vercel.app/

## Overview
- Anti-cheat comportamental, leve e server-side para SA-MP/open.mp
- Separa coleta, detecção, acumulação de evidências, decisão e logs
- Baixo falso positivo com correlação de eventos e período de graça

## Design Principles
- Clareza e previsibilidade em punições
- Performance O(1) por evento e decaimento em timer global
- Compatibilidade com GM legadas (wrappers e thresholds)

## Architecture
- Event Collection: AC_InitPlayer, AC_OnKeys, AC_OnMovement, AC_OnCommand
- Detection: regras simples e robustas por tipo de evento
- Evidence: correlação em janela curta e pontuação por categoria
- Decision: ShouldKick/ShouldBan respeitando período de graça e flags
- Logging: agregação e colapso de eventos repetidos

## Scores & Decision
- Categorias: SCORE_MACRO, SCORE_BOT, SCORE_SCRIPT
- Thresholds legados: AC_FLAG_SUSPEITO, AC_FLAG_KICK, AC_FLAG_BAN
- ShouldKick/ShouldBan verificam score total e flags de categoria

## Event Correlation
- Janela de correlação: eventos próximos e diferentes pontuam em dobro
- Evita punição por um único evento isolado

## Decay
- Não-linear: maior score decai lentamente, menor score rapidamente
- Executado por AC_Process em timer global

## Logging
- Resumo agregado com contagem e pontos por razão
- Log imediato opcional para debugging

## Integration
- OnPlayerConnect: AC_InitPlayer(playerid)
- OnPlayerKeyStateChange: AC_UpdateKeys(playerid, newkeys, oldkeys)
- OnPlayerUpdate: AC_UpdateMovement(playerid)
- Após executar um comando: AC_UpdateCommand(playerid)
- Timer global: chamar AC_Process() a cada 500 ms

## Public API
- Coleta: AC_InitPlayer, AC_OnKeys, AC_OnMovement, AC_OnCommand
- Wrappers: AC_UpdateKeys, AC_UpdateMovement, AC_UpdateCommand
- Process: AC_Process
- Score: AC_GetScoreCat, AC_GetScore
- Decision: AC_ShouldKick, AC_ShouldBan, AC_ShouldPunish
- Logs: AC_LogDump

## Configuration
- Intervalos: AC_PROCESS_INTERVAL_MS, AC_CORRELATION_WINDOW
- Graça: AC_GRACE_SECONDS
- Scores base: AC_SCORE_MACRO_LEVE, AC_SCORE_MACRO_EVIDENTE, AC_SCORE_SCRIPT_CLARO
- Logs: AC_LOG_ENABLED, AC_LOG_SIZE, AC_LOG_COLLAPSE_WINDOW, AC_LOG_IMMEDIATE

## Performance
- O(1) por evento, sem loops pesados por player
- AC_Process centraliza decaimento e verificações periódicas

## Compatibility
- Teclas mapeadas: KEY_AIM, KEY_LEFT/RIGHT/UP/DOWN
- Thresholds legados e wrappers para GMs existentes

