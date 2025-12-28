#include <a_samp>
#include <ac>

#define COLOR_RED 0xFF0000FF

// ===============================
// TIMER GLOBAL DO ANTICHEAT
// ===============================
forward AC_Tick();
public AC_Tick()
{
    AC_Process();
    return 1;
}

// ===============================
// GAMEMODE
// ===============================
public OnGameModeInit()
{
    print("Gamemode exemplo - AC Anticheat Comportamental");

    // Timer global (NUNCA crie por player)
    SetTimer("AC_Tick", 500, true);

    return 1;
}

public OnGameModeExit()
{
    return 1;
}

// ===============================
// PLAYER
// ===============================
public OnPlayerConnect(playerid)
{
    AC_InitPlayer(playerid);
    SendClientMessage(playerid, -1, "AC Anticheat carregado.");
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    return 1;
}

// ===============================
// COLETA DE EVENTOS
// ===============================
public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    AC_UpdateKeys(playerid, newkeys, oldkeys);

    // Decisão centralizada
    if (AC_ShouldBan(playerid))
    {
        SendClientMessage(playerid, COLOR_RED, "Cheat detectado (BAN).");
        Ban(playerid);
    }
    else if (AC_ShouldKick(playerid))
    {
        SendClientMessage(playerid, COLOR_RED, "Cheat detectado (KICK).");
        Kick(playerid);
    }

    return 1;
}

public OnPlayerUpdate(playerid)
{
    AC_UpdateMovement(playerid);
    return 1;
}

// ===============================
// COMANDOS
// ===============================
public OnPlayerCommandText(playerid, cmdtext[])
{
    // Exemplo de comando simples
    if (!strcmp(cmdtext, "/teste", true))
    {
        SendClientMessage(playerid, -1, "Comando executado.");
        AC_UpdateCommand(playerid);
        return 1;
    }

    return 0;
}