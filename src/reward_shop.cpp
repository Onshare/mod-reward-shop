/*
数据库操作：
1=给予物品
2=给予金币
3=更改名称
4=更改阵营
5=更改种族
*/

#include "Configuration/Config.h"
#include "Player.h"
#include "Creature.h"
#include "AccountMgr.h"
#include "ScriptMgr.h"
#include "GossipDef.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "Chat.h"

class reward_shop : public CreatureScript {
public:
    reward_shop() : CreatureScript("reward_shop") {}

    bool OnGossipHello(Player *player, Creature *creature) override {
        if (player->IsInCombat())
            return false;

        if (!sConfigMgr->GetOption<bool>("RewardShopEnable", 0))
            return false;


        std::string text = "点击接受然后输入兑换码";
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "我想兑换我的兑换码.", GOSSIP_SENDER_MAIN, 1, text, 0, true);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "什么是兑换码?我该怎么获得?", GOSSIP_SENDER_MAIN, 2);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "我没有兑换码", GOSSIP_SENDER_MAIN, 3);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "我要领取我的招募奖励.", GOSSIP_SENDER_MAIN, 11, "点击接受，输入被招募账号的注册邮件", 0, true);
        if (sConfigMgr->GetOption<bool>("AllowGM", 1))
            if (player->IsGameMaster())
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "[GM]生成一个兑换码.", GOSSIP_SENDER_MAIN, 4);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }


    bool OnGossipSelect(Player *player, Creature *creature, uint32, uint32 action) override {
        player->PlayerTalkClass->ClearMenus();
        auto info = sConfigMgr->GetOption<std::string>("WebsiteAddress", "特殊活动期间获得,敬请期待...");
        uint32 rnd1 = urand(10000, 90000);
        uint32 rnd2 = urand(10000, 90000);
        uint32 rnd3 = urand(10000, 90000);
        uint32 rnd4 = urand(10000, 90000);
        uint32 rnd5 = urand(10000, 90000);

        std::string CreatedBy = player->GetName();
        std::ostringstream randomcode;
        randomcode << rnd1 << "-" << rnd2 << "-" << rnd3 << "-" << rnd4 << "-" << rnd5;

        switch (action) {
            case 2:
                creature->Whisper(info.c_str(), LANG_UNIVERSAL, player, false);
                CloseGossipMenuFor(player);
                break;
            case 3:
                CloseGossipMenuFor(player);
                break;
            case 4:
                player->PlayerTalkClass->ClearMenus();
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "生成更改名字兑换码.", GOSSIP_SENDER_MAIN, 6);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "生成更改阵营兑换码.", GOSSIP_SENDER_MAIN, 7);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "生成更改种族兑换码.", GOSSIP_SENDER_MAIN, 8);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "生成角色直升兑换码.", GOSSIP_SENDER_MAIN, 9);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "暂时不需要生成任何兑换码.", GOSSIP_SENDER_MAIN, 10);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                break;
            case 6:
                CharacterDatabase.Query(
                        "INSERT INTO `reward_shop` (`action`, `action_data`, `quantity`, `code`, `status`, `PlayerGUID`, `PlayerIP`, `CreatedBy`) VALUES(3, 0, 0, '{}', 0, 0, '0', '{}')",
                        randomcode.str().c_str(), CreatedBy.c_str());
                ChatHandler(player->GetSession()).PSendSysMessage("已成功创建,更改名字兑换码是 %s",
                                                                  randomcode.str().c_str());
                CloseGossipMenuFor(player);
                break;
            case 7:
                CharacterDatabase.Query(
                        "INSERT INTO `reward_shop` (`action`, `action_data`, `quantity`, `code`, `status`, `PlayerGUID`, `PlayerIP`,`CreatedBy`) VALUES(4, 0, 0, '{}', 0, 0, '0', '{}')",
                        randomcode.str().c_str(), CreatedBy.c_str());
                ChatHandler(player->GetSession()).PSendSysMessage("已成功创建,更改阵营兑换码是 %s",
                                                                  randomcode.str().c_str());
                CloseGossipMenuFor(player);
                break;
            case 8:
                CharacterDatabase.Query(
                        "INSERT INTO `reward_shop` (`action`, `action_data`, `quantity`, `code`, `status`, `PlayerGUID`, `PlayerIP`, `CreatedBy`) VALUES(5, 0, 0, '{}', 0, 0, '0', '{}')",
                        randomcode.str().c_str(), CreatedBy.c_str());
                ChatHandler(player->GetSession()).PSendSysMessage("已成功创建,更改种族兑换码是 %s",
                                                                  randomcode.str().c_str());
                CloseGossipMenuFor(player);
                break;
            case 9:
                CharacterDatabase.Query(
                        "INSERT INTO `reward_shop` (`action`, `action_data`, `quantity`, `code`, `status`, `PlayerGUID`, `PlayerIP`, `CreatedBy`) VALUES(6, 0, 0, '{}', 0, 0, '0', '{}')",
                        randomcode.str().c_str(), CreatedBy.c_str());
                ChatHandler(player->GetSession()).PSendSysMessage("已成功创建,角色直升兑换码是 %s",
                                                                  randomcode.str().c_str());
                CloseGossipMenuFor(player);
                break;
            default:
                CloseGossipMenuFor(player);
                break;
        }
        return true;
    }

    bool OnGossipSelectCode(Player *player, Creature *creature, uint32, uint32 action, const char *code) override {
        if(action == 11) {
            uint8 level = player->getLevel();
            if (level < 80) {
                ChatHandler(player->GetSession()).PSendSysMessage("未满80级，无法领取招募奖励。");
                CloseGossipMenuFor(player);
                return false;
            }
            uint32 guid = player->GetSession()->GetAccountId();
            QueryResult accountResult = LoginDatabase.Query("SELECT username FROM account WHERE id={}", guid);
            if(!accountResult) {
                CloseGossipMenuFor(player);
                return false;
            }
            Field* fields = accountResult->Fetch();
            std::string username = fields[0].Get<std::string>();
            if(username.empty()) {
                CloseGossipMenuFor(player);
                return false;
            }
            std::string friendEmail = code;
            QueryResult result = LoginDatabase.Query("SELECT * FROM recruitment WHERE inviter='{}' AND friend='{}' AND status = {} AND reward = {}", username, friendEmail.c_str(), 1, 0);
            if(!result) {
                ChatHandler(player->GetSession()).PSendSysMessage("奖励条件不符合.");
                CloseGossipMenuFor(player);
                return false;
            }
            LoginDatabase.Query("UPDATE recruitment SET reward={} WHERE inviter='{}' AND friend='{}'", 1, username, friendEmail.c_str());
            player->AddItem(69999, 50);
            player->AddItem(38082, 4);
            ChatHandler(player->GetSession()).PSendSysMessage("招募奖励领取完成.");
            CloseGossipMenuFor(player);
            return true;
        }
        ObjectGuid playerguid = player->GetGUID();
        std::string playerIP = player->GetSession()->GetRemoteAddress();
        std::string rewardcode = code;
        std::ostringstream messageCode;
        messageCode << "对不起 " << player->GetName() << ", 您的兑换码不是有效兑换码或已兑换.";

        //检查代码
        QueryResult result = CharacterDatabase.Query(
                "SELECT id, action, action_data, quantity, status FROM reward_shop WHERE code = '{}' AND status = {}",
                rewardcode.c_str(), 0);
        std::size_t found = rewardcode.find_first_not_of(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890-");
        if (found != std::string::npos)
            return false;

        if (!result) {
            player->PlayDirectSound(9638); // 不
            creature->Whisper(messageCode.str(), LANG_UNIVERSAL, player);
            creature->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            return false;
        }

        std::transform(rewardcode.begin(), rewardcode.end(), rewardcode.begin(), ::toupper);

        do {
            Field *fields = result->Fetch();
//            uint32 id = fields[0].Get<uint32>();
            uint32 action = fields[1].Get<uint32>();
            int32 action_data = fields[2].Get<int32>();
            uint32 quantity = fields[3].Get<uint32>();
            uint32 status = fields[4].Get<uint32>();
            int count = 1;
            uint32 noSpaceForCount = 0;
            ItemPosCountVec dest;
            InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, action_data, quantity,
                                                          &noSpaceForCount);
            if (status == 1) {
                player->PlayDirectSound(9638); // No
                creature->Whisper(messageCode.str(), LANG_UNIVERSAL, player);
                creature->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                return false;
            }
            switch (action) {
                case 1: /* 兑换道具 */
                    if (msg != EQUIP_ERR_OK)
                        count -= noSpaceForCount;

                    if (count == 0 || dest.empty()) {
                        ChatHandler(player->GetSession()).PSendSysMessage("无法完成兑换,因为物品是唯一或没有足够空间.");
                        ChatHandler(player->GetSession()).SetSentErrorMessage(true);
                    } else if (count > 0 && action_data) {
                        player->AddItem(action_data, quantity);
                    }
                    break;
                case 2: /* 兑换金币 */
                    player->ModifyMoney(action_data * 10000);
                    ChatHandler(player->GetSession()).PSendSysMessage("兑换码成功，已兑换 %u金币", action_data);
                    break;
                case 3: /* 名称更改 */
                    player->SetAtLoginFlag(AT_LOGIN_RENAME);
                    ChatHandler(player->GetSession()).PSendSysMessage("兑换成功，请退出到角色选择页面完成更改名称.");
                    break;
                case 4: /* 阵营变化 */
                    player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
                    ChatHandler(player->GetSession()).PSendSysMessage("兑换成功，请退出到角色选择页面完成更改阵营.");
                    break;
                case 5: /* 种族变化 */
                    player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
                    ChatHandler(player->GetSession()).PSendSysMessage("兑换成功，请退出到角色选择页面完成更改种族.");
                    break;
                case 6://直升
                    player->GiveLevel(80);
                    player->AddItem(69999, 50);
                    player->AddItem(38082, 4);
                    ChatHandler(player->GetSession()).PSendSysMessage("兑换成功，角色等级升级到80级.");
                    break;
            }

        } while (result->NextRow());

        CharacterDatabase.Query(
                "UPDATE reward_shop SET status = 1, PlayerGUID = '{}', PlayerIP = '{}' WHERE code = '{}'", playerguid.GetRawValue(),
                playerIP.c_str(), rewardcode.c_str());
        CloseGossipMenuFor(player);
        return true;
    }


    struct npc_reward_shopAI : public ScriptedAI {
        explicit npc_reward_shopAI(Creature *creature) : ScriptedAI(creature) {}

        uint32 say_timer{};
        bool canSay{};

        void Reset() override {
            say_timer = 1000;
            canSay = false;
        }

        void MoveInLineOfSight(Unit *who) override {
            if (me->IsWithinDist(who, 5.0f) && who->GetTypeId() == TYPEID_PLAYER) {
                canSay = true;
            } else
                canSay = false;
        }

        void UpdateAI(uint32 diff) override {
            if (say_timer <= diff) {
                if (canSay) {
                    me->Say("欢迎!你想看看一些神奇的商品吗？", LANG_UNIVERSAL);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                    say_timer = 61000;
                }
            } else
                say_timer -= diff;
        }

    };

    CreatureAI *GetAI(Creature *creature) const override {
        return new npc_reward_shopAI(creature);
    }
};


void AddRewardShopScripts() {
    new reward_shop();
}
