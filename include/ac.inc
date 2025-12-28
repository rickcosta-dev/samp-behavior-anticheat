/*
    AC Anticheat Comportamental — ac.inc
    Autor: RickZin (Discord: rickzin021)
    Links: GitHub https://github.com/rickcosta-dev • Portfólio https://rickzindev.vercel.app/
    Propósito: detecção comportamental leve, com baixo falso positivo e integração simples.
    Nota: acione AC_Process a cada 500 ms e chame AC_Update* nos callbacks da sua GM.
*/
#define AC_SCORE_MACRO_LEVE        10
#define AC_SCORE_MACRO_EVIDENTE    25
#define AC_SCORE_SCRIPT_CLARO      40

#define AC_MIN_HUMAN_REACTION      80
#define AC_INTERVAL_TOLERANCE      3
#define AC_POS_TOLERANCE           0.005
#define AC_ANGLE_STABLE_TOL        0.1

#define AC_PROCESS_INTERVAL_MS     500
#define AC_GRACE_SECONDS           20
#define AC_CORRELATION_WINDOW      1500
#define AC_FLAG_CLEAR_WINDOW       15000
#define AC_NOISE_SPREAD_MS         3
#define AC_STABLE_MOVE_STEPS       75

#define AC_LOG_ENABLED             1
#define AC_LOG_SIZE                16
#define AC_LOG_COLLAPSE_WINDOW     2000
#define AC_LOG_IMMEDIATE           0

#define AC_FLAG_SUSPEITO           40
#define AC_FLAG_KICK               70
#define AC_FLAG_BAN                100


#if !defined KEY_AIM
    #define KEY_AIM KEY_SECONDARY_ATTACK
#endif
#if !defined KEY_LEFT
    #define KEY_LEFT KEY_LOOK_LEFT
#endif
#if !defined KEY_RIGHT
    #define KEY_RIGHT KEY_LOOK_RIGHT
#endif
#if !defined KEY_UP
    #define KEY_UP KEY_ANALOG_UP
#endif
#if !defined KEY_DOWN
    #define KEY_DOWN KEY_ANALOG_DOWN
#endif


#define AC_REASON_KEYS          1
#define AC_REASON_TURN_FIRE     2
#define AC_REASON_MOVE_PERFECT  3
#define AC_REASON_CMD_INTERVAL  4
#define AC_REASON_REACTION      5


#define AC_FLAG_MACRO   (1 << 0)
#define AC_FLAG_BOT     (1 << 1)
#define AC_FLAG_SCRIPT  (1 << 2)

#define SCORE_MACRO     0
#define SCORE_BOT       1
#define SCORE_SCRIPT    2
#define SCORE_MAX       3

enum e_ac_data {
    ac_last_action,
    ac_last_keys,
    ac_last_move_tick,
    ac_repeat_count
};

new AC_Data[MAX_PLAYERS][e_ac_data];
new AC_Score[MAX_PLAYERS][SCORE_MAX];

new Float:AC_LastPosX[MAX_PLAYERS];
new Float:AC_LastPosY[MAX_PLAYERS];
new Float:AC_LastPosZ[MAX_PLAYERS];
new Float:AC_LastAngle[MAX_PLAYERS];
new Float:AC_LastDeltaDist[MAX_PLAYERS];

new AC_LastCmdTick[MAX_PLAYERS];
new AC_LastInterval[MAX_PLAYERS];
new AC_RepeatIntervalCount[MAX_PLAYERS];

new AC_PerfectMoveCount[MAX_PLAYERS];
new AC_LastKeysTick[MAX_PLAYERS];
new AC_LastDecayTick[MAX_PLAYERS];
new AC_InitTick[MAX_PLAYERS];
new AC_IsMoving[MAX_PLAYERS];

new AC_Flags[MAX_PLAYERS];
new AC_LastSuspTick[MAX_PLAYERS];


new AC_LastEventCode[MAX_PLAYERS];
new AC_LastEventTick[MAX_PLAYERS];
new AC_PendingCode[MAX_PLAYERS];
new AC_PendingTick[MAX_PLAYERS];
new AC_PendingCategory[MAX_PLAYERS];
new AC_PendingPoints[MAX_PLAYERS];


new AC_LogCode[MAX_PLAYERS][AC_LOG_SIZE];
new AC_LogCount[MAX_PLAYERS][AC_LOG_SIZE];
new AC_LogPoints[MAX_PLAYERS][AC_LOG_SIZE];
new AC_LogTick[MAX_PLAYERS][AC_LOG_SIZE];
new AC_LogMeasure[MAX_PLAYERS][AC_LOG_SIZE];
new AC_LogThresh[MAX_PLAYERS][AC_LOG_SIZE];
new AC_LogHead[MAX_PLAYERS];
new AC_LastFlagLevel[MAX_PLAYERS];


stock Float:AC__AngleDiff(Float:a, Float:b);
stock AC_InitPlayer(playerid);
stock AC_OnCommand(playerid);
stock AC_OnKeys(playerid, newkeys, oldkeys);
stock AC_OnMovement(playerid);
stock AC_Process(); // timer global
stock AC_GetScoreCat(playerid, category);
stock AC_GetScore(playerid);
stock bool:AC_ShouldKick(playerid);
stock bool:AC_ShouldBan(playerid);
stock bool:AC_ShouldPunish(playerid);
stock AC_LogDump(playerid);


stock AC_UpdateKeys(playerid, newkeys, oldkeys) { return AC_OnKeys(playerid, newkeys, oldkeys); }
stock AC_UpdateMovement(playerid) { return AC_OnMovement(playerid); }
stock AC_UpdateCommand(playerid) { return AC_OnCommand(playerid); }


#define AC_GetScore(%0,%1) AC_GetScoreCat(%0,%1)


stock AC_InitPlayer(playerid)
{
    AC_Data[playerid][ac_last_action] = GetTickCount();
    AC_Data[playerid][ac_last_keys] = 0;
    AC_Data[playerid][ac_last_move_tick] = GetTickCount();
    AC_Data[playerid][ac_repeat_count] = 0;

    AC_InitTick[playerid] = GetTickCount();

    AC_LastPosX[playerid] = 0.0;
    AC_LastPosY[playerid] = 0.0;
    AC_LastPosZ[playerid] = 0.0;
    AC_LastAngle[playerid] = 0.0;
    AC_LastDeltaDist[playerid] = -1.0;

    AC_LastCmdTick[playerid] = 0;
    AC_LastInterval[playerid] = 0;
    AC_RepeatIntervalCount[playerid] = 0;

    AC_PerfectMoveCount[playerid] = 0;
    AC_LastKeysTick[playerid] = 0;
    AC_LastDecayTick[playerid] = GetTickCount();
    AC_IsMoving[playerid] = 0;

    AC_Flags[playerid] = 0;
    AC_LastSuspTick[playerid] = 0;

    AC_LastEventCode[playerid] = 0;
    AC_LastEventTick[playerid] = 0;
    AC_PendingCode[playerid] = 0;
    AC_PendingTick[playerid] = 0;
    AC_PendingCategory[playerid] = 0;
    AC_PendingPoints[playerid] = 0;

    for (new c = 0; c < _:SCORE_MAX; c++) AC_Score[playerid][c] = 0;

    AC_LogHead[playerid] = 0;
    AC_LastFlagLevel[playerid] = 0;
    for (new i = 0; i < AC_LOG_SIZE; i++)
    {
        AC_LogCode[playerid][i] = 0;
        AC_LogCount[playerid][i] = 0;
        AC_LogPoints[playerid][i] = 0;
        AC_LogTick[playerid][i] = 0;
        AC_LogMeasure[playerid][i] = 0;
        AC_LogThresh[playerid][i] = 0;
    }
    return 1;
}


stock AC_OnKeys(playerid, newkeys, oldkeys)
{
    new now = GetTickCount();
    AC__MaybeClearFlags(playerid, now);

    new toggled = newkeys ^ oldkeys;
    new changes = AC__PopCount(toggled);
    if (changes >= 3)
    {
        AC_Data[playerid][ac_repeat_count]++;
        AC__ConsiderEvidence(playerid, SCORE_MACRO, AC_SCORE_MACRO_LEVE, AC_REASON_KEYS, changes, 3, now);
    }

    new Float:angle;
    GetPlayerFacingAngle(playerid, angle);
    new Float:delta = AC__AngleDiff(AC_LastAngle[playerid], angle);

    if ((newkeys & KEY_FIRE))
    {
        new thresh = 170 + AC__Noise(playerid, 0, 2);
        if (floatround(delta) >= thresh && (now - AC_Data[playerid][ac_last_move_tick] <= 30))
        {
            AC_Data[playerid][ac_repeat_count]++;
            AC__ConsiderEvidence(playerid, SCORE_SCRIPT, AC_SCORE_SCRIPT_CLARO, AC_REASON_TURN_FIRE, floatround(delta), thresh, now);
        }
    }

    AC_LastAngle[playerid] = angle;
    AC_Data[playerid][ac_last_keys] = newkeys;
    AC_LastKeysTick[playerid] = now;
    return 1;
}

stock AC_OnMovement(playerid)
{
    new now = GetTickCount();
    new Float:x, Float:y, Float:z, Float:angle;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angle);

    if (AC_Data[playerid][ac_last_move_tick] != 0)
    {
        new Float:dx = x - AC_LastPosX[playerid];
        new Float:dy = y - AC_LastPosY[playerid];
        new Float:dz = z - AC_LastPosZ[playerid];
        new Float:dist = floatsqroot(dx*dx + dy*dy + dz*dz);
        AC_IsMoving[playerid] = (dist > 0.001);
        AC_LastDeltaDist[playerid] = dist;
    }

    AC_LastPosX[playerid] = x;
    AC_LastPosY[playerid] = y;
    AC_LastPosZ[playerid] = z;
    AC_LastAngle[playerid] = angle;
    AC_Data[playerid][ac_last_move_tick] = now;
    return 1;
}

stock AC_OnCommand(playerid)
{
    new now = GetTickCount();
    AC__MaybeClearFlags(playerid, now);

    if (AC_LastCmdTick[playerid] > 0)
    {
        new interval = now - AC_LastCmdTick[playerid];
        new tol = AC_INTERVAL_TOLERANCE + AC__Noise(playerid, 0, AC_NOISE_SPREAD_MS);

        if (AC_LastInterval[playerid] > 0 && AC__Abs(interval - AC_LastInterval[playerid]) <= tol)
        {
            AC_RepeatIntervalCount[playerid]++;
        }
        else
        {
            AC_RepeatIntervalCount[playerid] = 1;
        }

        if (AC_RepeatIntervalCount[playerid] >= 4)
        {
            AC_Data[playerid][ac_repeat_count]++;
            AC__ConsiderEvidence(playerid, SCORE_MACRO, AC_SCORE_MACRO_EVIDENTE, AC_REASON_CMD_INTERVAL, interval, tol, now);
        }

        AC_LastInterval[playerid] = interval;
    }

    new r1 = (AC_LastKeysTick[playerid] > 0) ? (now - AC_LastKeysTick[playerid]) : 999999;
    new r2 = (AC_Data[playerid][ac_last_move_tick] > 0) ? (now - AC_Data[playerid][ac_last_move_tick]) : 999999;
    new reaction = (r1 < r2) ? r1 : r2;
    new rt = AC_MIN_HUMAN_REACTION - AC__Noise(playerid, 0, 5);

    if (reaction < rt)
    {
        AC_Data[playerid][ac_repeat_count]++;
        AC__ConsiderEvidence(playerid, SCORE_BOT, AC_SCORE_MACRO_EVIDENTE, AC_REASON_REACTION, reaction, rt, now);
    }

    AC_LastCmdTick[playerid] = now;
    AC_Data[playerid][ac_last_action] = now;
    return 1;
}

// Processamento Timer

stock AC_Process()
{
    new now = GetTickCount();

    for (new playerid = 0; playerid < MAX_PLAYERS; playerid++)
    {
        if (AC_LastDecayTick[playerid] == 0) continue;

        if (now - AC_LastDecayTick[playerid] >= AC_PROCESS_INTERVAL_MS)
        {
            AC__DecayAll(playerid);
            AC_LastDecayTick[playerid] = now;
        }

        
        if (AC_IsMoving[playerid] && AC_LastDeltaDist[playerid] >= 0.0)
        {
            new Float:dist = AC_LastDeltaDist[playerid];
          
            if (floatabs(dist - AC_LastDeltaDist[playerid]) <= AC_POS_TOLERANCE)
            {
                AC_PerfectMoveCount[playerid]++;
                if (AC_PerfectMoveCount[playerid] == AC_STABLE_MOVE_STEPS)
                {
                    new measure = floatround(dist * 1000.0);
                    new thresh = floatround(AC_POS_TOLERANCE * 1000.0);
                    AC__ConsiderEvidence(playerid, SCORE_SCRIPT, AC_SCORE_MACRO_LEVE, AC_REASON_MOVE_PERFECT, measure, thresh, now);
                }
            }
            else
            {
                AC_PerfectMoveCount[playerid] = 0;
            }
        }

        AC__MaybeClearFlags(playerid, now);
    }
    return 1;
}

// Decisão

stock AC_GetScoreCat(playerid, category)
{
    if (category < 0 || category >= SCORE_MAX) return 0;
    return AC_Score[playerid][category];
}

stock AC_GetScore(playerid)
{
    return AC_Score[playerid][SCORE_MACRO]
        + AC_Score[playerid][SCORE_BOT]
        + AC_Score[playerid][SCORE_SCRIPT];
}

stock bool:AC_ShouldKick(playerid)
{
    new now = GetTickCount();
    if ((now - AC_InitTick[playerid]) / 1000 < AC_GRACE_SECONDS) return false;

    new total = AC_Score[playerid][SCORE_MACRO] + AC_Score[playerid][SCORE_BOT] + AC_Score[playerid][SCORE_SCRIPT];
    new kmin = 70 + AC__Noise(playerid, 0, 3);

    if (total >= kmin && (AC_Flags[playerid] & (AC_FLAG_MACRO | AC_FLAG_SCRIPT))) return true;
    if (AC_Score[playerid][SCORE_SCRIPT] >= kmin) return true;
    return false;
}

stock bool:AC_ShouldBan(playerid)
{
    new now = GetTickCount();
    if ((now - AC_InitTick[playerid]) / 1000 < AC_GRACE_SECONDS) return false;

    new total = AC_Score[playerid][SCORE_MACRO] + AC_Score[playerid][SCORE_BOT] + AC_Score[playerid][SCORE_SCRIPT];
    new bmin = 100 + AC__Noise(playerid, 0, 5);

    if (total >= bmin && (AC_Flags[playerid] & AC_FLAG_SCRIPT)) return true;
    if (AC_Score[playerid][SCORE_SCRIPT] >= bmin) return true;
    return false;
}

stock bool:AC_ShouldPunish(playerid)
{
    return AC_ShouldKick(playerid) || AC_ShouldBan(playerid);
}


stock AC__PopCount(value)
{
    new c = 0, v = value;
    while (v)
    {
        c += (v & 1);
        v >>= 1;
    }
    return c;
}

stock AC__Abs(v)
{
    return (v < 0) ? -v : v;
}

stock Float:AC__AngleDiff(Float:a, Float:b)
{
    new Float:d = floatabs(a - b);
    if (d > 180.0) d = 360.0 - d;
    return d;
}

stock AC__DecayAll(playerid)
{
    for (new c = 0; c < SCORE_MAX; c++)
    {
        new s = AC_Score[playerid][c];
        if (s > 0)
        {
            new d = AC__DecayAmount(s);
            AC_Score[playerid][c] = (s > d) ? (s - d) : 0;
        }
    }
}

stock AC__DecayAmount(score)
{
    if (score <= 20) return 3; // baixo → rápido
    if (score <= 40) return 2; // médio → moderado
    return 1;                  // alto → lento
}

stock AC__Noise(playerid, base, spread)
{
    if (spread <= 0) return base;
    new n = random(spread * 2 + 1) - spread;
    return base + (n ^ playerid) % (spread + 1);
}

stock AC__ConsiderEvidence(playerid, category, basePoints, code, measure, baseThresh, now)
{
    if (AC_PendingCode[playerid] == 0)
    {
        AC_PendingCode[playerid] = code;
        AC_PendingTick[playerid] = now;
        AC_PendingCategory[playerid] = category;
        AC_PendingPoints[playerid] = basePoints;
        return 1;
    }

    if (now - AC_PendingTick[playerid] <= AC_CORRELATION_WINDOW)
    {
        new points = AC_PendingPoints[playerid];
        if (AC_PendingCode[playerid] != code) points *= 2; // eventos diferentes → multiplicar
        AC__Award(playerid, AC_PendingCategory[playerid], points, code, measure, baseThresh, now);
        AC_PendingCode[playerid] = 0;
        AC_PendingTick[playerid] = 0;
        AC_PendingCategory[playerid] = 0;
        AC_PendingPoints[playerid] = 0;
    }
    else
    {
        AC_PendingCode[playerid] = code;
        AC_PendingTick[playerid] = now;
        AC_PendingCategory[playerid] = category;
        AC_PendingPoints[playerid] = basePoints;
    }
    return 1;
}

stock AC__AddScoreCat(playerid, category, amount)
{
    if (category < 0 || category >= SCORE_MAX) return 0;
    AC_Score[playerid][category] += amount;
    if (AC_Score[playerid][category] > 100) AC_Score[playerid][category] = 100;
    return 1;
}

stock AC__Award(playerid, category, points, code, measure, thresh, now)
{
    AC__AddScoreCat(playerid, category, points);

    if (category == SCORE_MACRO) AC_Flags[playerid] |= AC_FLAG_MACRO;
    else if (category == SCORE_BOT) AC_Flags[playerid] |= AC_FLAG_BOT;
    else if (category == SCORE_SCRIPT) AC_Flags[playerid] |= AC_FLAG_SCRIPT;

    AC_LastSuspTick[playerid] = now;
    AC__LogEvent(playerid, points, code, measure, thresh, now);

    new total = AC_Score[playerid][SCORE_MACRO] + AC_Score[playerid][SCORE_BOT] + AC_Score[playerid][SCORE_SCRIPT];
    new prev = AC_LastFlagLevel[playerid];
    new lvl = AC__FlagLevel(total);
    if (lvl > prev)
    {
        #if AC_LOG_ENABLED
        AC_LogDump(playerid);
        printf("[AC] jogador %d level=%d total=%d", playerid, lvl, total);
        #endif
        AC_LastFlagLevel[playerid] = lvl;
    }
}

stock AC__MaybeClearFlags(playerid, now)
{
    if (AC_LastSuspTick[playerid] == 0) return 1;
    if (now - AC_LastSuspTick[playerid] > AC_FLAG_CLEAR_WINDOW)
    {
        AC_Flags[playerid] = 0;
        AC_LastSuspTick[playerid] = 0;
    }
    return 1;
}

stock AC__ReasonText(code, dest[], size)
{
    switch (code)
    {
        case AC_REASON_KEYS:         format(dest, size, "Transicao de teclas anormal");
        case AC_REASON_TURN_FIRE:    format(dest, size, "Virar 180 e atirar no mesmo tick");
        case AC_REASON_MOVE_PERFECT: format(dest, size, "Movimento continuo perfeito");
        case AC_REASON_CMD_INTERVAL: format(dest, size, "Comandos em intervalo constante");
        case AC_REASON_REACTION:     format(dest, size, "Reacao abaixo do humano");
        default:                     format(dest, size, "Motivo desconhecido");
    }
    return 1;
}

stock AC__FlagLevel(score)
{

    if (score >= AC_SCORE_SCRIPT_CLARO + AC_SCORE_MACRO_EVIDENTE) return 3;
    if (score >= AC_SCORE_MACRO_EVIDENTE) return 2;
    if (score >= AC_SCORE_MACRO_LEVE) return 1;
    return 0;
}

stock AC__LogEvent(playerid, points, code, measure, thresh, now)
{
    new head = AC_LogHead[playerid];
    new prev = (head + AC_LOG_SIZE - 1) % AC_LOG_SIZE;

    if (AC_LogCode[playerid][prev] == code && now - AC_LogTick[playerid][prev] <= AC_LOG_COLLAPSE_WINDOW)
    {
        AC_LogCount[playerid][prev]++;
        AC_LogPoints[playerid][prev] += points;
        AC_LogTick[playerid][prev] = now;
        AC_LogMeasure[playerid][prev] = measure;
        AC_LogThresh[playerid][prev] = thresh;
    }
    else
    {
        AC_LogCode[playerid][head] = code;
        AC_LogCount[playerid][head] = 1;
        AC_LogPoints[playerid][head] = points;
        AC_LogTick[playerid][head] = now;
        AC_LogMeasure[playerid][head] = measure;
        AC_LogThresh[playerid][head] = thresh;
        AC_LogHead[playerid] = (head + 1) % AC_LOG_SIZE;
    }

    #if AC_LOG_ENABLED && AC_LOG_IMMEDIATE
    new txt[64];
    AC__ReasonText(code, txt, sizeof(txt));
    new total = AC_Score[playerid][SCORE_MACRO] + AC_Score[playerid][SCORE_BOT] + AC_Score[playerid][SCORE_SCRIPT];
    printf("[AC] player=%d tipo=%s val=%d thr=%d +%d total=%d", playerid, txt, measure, thresh, points, total);
    #endif
}

stock AC_LogDump(playerid)
{
    new counts[8];
    new points[8];

    for (new i = 0; i < AC_LOG_SIZE; i++)
    {
        new c = AC_LogCode[playerid][i];
        if (c >= 1 && c <= 5)
        {
            counts[c] += AC_LogCount[playerid][i];
            points[c] += AC_LogPoints[playerid][i];
        }
    }

    #if AC_LOG_ENABLED
    new total = AC_Score[playerid][SCORE_MACRO] + AC_Score[playerid][SCORE_BOT] + AC_Score[playerid][SCORE_SCRIPT];
    printf("[AC] jogador %d resumo total=%d macro=%d bot=%d script=%d",
        playerid, total, AC_Score[playerid][SCORE_MACRO], AC_Score[playerid][SCORE_BOT], AC_Score[playerid][SCORE_SCRIPT]);

    new txt[64];
    for (new r = 1; r <= 5; r++)
    {
        if (counts[r] > 0)
        {
            AC__ReasonText(r, txt, sizeof(txt));
            printf("[AC] jogador %d %s x%d +%d", playerid, txt, counts[r], points[r]);
        }
    }
    #endif
    return 1;
}

stock AC__UseSAMPVersion()
{
    #if defined SAMP_INCLUDES_VERSION
        return SAMP_INCLUDES_VERSION;
    #else
        return 0;
    #endif
}
