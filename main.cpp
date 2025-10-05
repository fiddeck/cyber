#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <random>
#include <ctime>

#pragma comment(lib, "Comctl32.lib")

// 自定义按钮ID
const int BTN_EXPLORE = 1001;
const int BTN_FIGHT = 1002;
const int BTN_HACK = 1003;
const int BTN_EXIT = 1004;

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    // 初始化公共控件库
    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_STANDARD_CLASSES };
    InitCommonControlsEx(&icex);

    // 初始化随机数生成器
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int Button = 0;

    // 初始状态
    int score = 0;
    int energy = 100;
    int day = 1;

    // 欢迎一次性提示
    TaskDialog(NULL, NULL, L"Hello World！", L"赛博朋克冒险已启动。选择你的行动，管理能量并获取分数。", L"祝你好运，夜之城在等你。", TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);

    // 主循环：直到用户选择退出或能量耗尽
    bool running = true;
    while (running)
    {
        // 如果体力耗尽 -> 结束游戏
        if (energy <= 0)
        {
            TaskDialog(NULL, NULL, L"游戏结束", L"你的能量耗尽，夜之城吞噬了你。", NULL, TDCBF_OK_BUTTON, TD_ERROR_ICON, NULL);
            break;
        }

        // 构造主菜单（命令链接）
        TASKDIALOGCONFIG tdc = { 0 };
        tdc.cbSize = sizeof(tdc);
        tdc.hwndParent = NULL;
        tdc.dwFlags = TDF_USE_COMMAND_LINKS | TDF_ALLOW_DIALOG_CANCELLATION;
        tdc.pszWindowTitle = L"夜之城: 随机事件";
        tdc.pszMainInstruction = L"你现在可以行动了";
        wchar_t buffer[256];
        swprintf_s(buffer, L"第 %d 天 — 能量: %d  分数: %d\n选择一项行动来继续你的冒险：", day, energy, score);
        tdc.pszContent = buffer;

        TASKDIALOG_BUTTON buttons[] = {
            { BTN_EXPLORE, L"探索(&E)\n在街区搜刮资源，风险与回报并存" },
            { BTN_FIGHT,   L"对战(&F)\n挑战一名对手，赢得名声或受伤" },
            { BTN_HACK,    L"入侵(&H)\n尝试黑入节点，快速获得积分" },
            { BTN_EXIT,    L"退出(&X)\n结束这一轮冒险" }
        };
        tdc.pButtons = buttons;
        tdc.cButtons = ARRAYSIZE(buttons);
        tdc.nDefaultButton = BTN_EXPLORE;
        tdc.pszMainIcon = TD_INFORMATION_ICON;

        TaskDialogIndirect(&tdc, &Button, NULL, NULL);

        switch (Button)
        {
        case BTN_EXPLORE:
        {
            // 随机事件
            int ev = std::rand() % 4;
            if (ev == 0)
            {
                int found = (std::rand() % 50) + 10;
                score += found;
                TaskDialog(NULL, NULL, L"探索：发现", L"你在废墟中找到了一些现金与零件。", NULL, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
                // 详细提示
                wchar_t msg[128];
                swprintf_s(msg, L"获得分数: +%d", found);
                TaskDialog(NULL, NULL, L"战利品", msg, NULL, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
            }
            else if (ev == 1)
            {
                int gain = (std::rand() % 20) + 5;
                energy += gain;
                if (energy > 150) energy = 150;
                wchar_t msg[128];
                swprintf_s(msg, L"你恢复了 %d 点能量（上限 150）", gain);
                TaskDialog(NULL, NULL, L"探索：补给", msg, NULL, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
            }
            else if (ev == 2)
            {
                // 遇到小偷/敌人，战斗判定
                int enemy = (std::rand() % 80) + 20;
                int chance = (std::rand() % 100) + energy / 3;
                if (chance > enemy)
                {
                    int reward = (std::rand() % 40) + 20;
                    score += reward;
                    energy -= 10;
                    wchar_t msg[128];
                    swprintf_s(msg, L"你击败了敌人！获得 %d 分，失去 10 能量。", reward);
                    TaskDialog(NULL, NULL, L"遭遇战：胜利", msg, NULL, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
                }
                else
                {
                    int loss = (std::rand() % 20) + 10;
                    energy -= loss;
                    wchar_t msg[128];
                    swprintf_s(msg, L"战斗失利，损失 %d 能量。", loss);
                    TaskDialog(NULL, NULL, L"遭遇战：落败", msg, NULL, TDCBF_OK_BUTTON, TD_WARNING_ICON, NULL);
                }
            }
            else // ev == 3
            {
                // 触发小事件：道具或谜语
                TaskDialog(NULL, NULL, L"奇遇", L"你遇到了一名神秘人，他给了你一段提示：'数字之外皆虚空'。", NULL, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
                // 小奖励
                score += 15;
            }
            day++;
            break;
        }
        case BTN_FIGHT:
        {
            // 简单三回合战斗模拟
            TaskDialog(NULL, NULL, L"擂台挑战", L"你进入了地下擂台，准备三回合战斗。", NULL, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
            int rounds = 3;
            int fightScore = 0;
            for (int r = 1; r <= rounds; ++r)
            {
                int playerRoll = (std::rand() % 100) + energy / 5;
                int enemyRoll = (std::rand() % 100);
                if (playerRoll >= enemyRoll)
                {
                    fightScore += 10;
                    TaskDialog(NULL, NULL, L"回合结果", L"你赢下了这一回合。", NULL, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
                }
                else
                {
                    energy -= 8;
                    TaskDialog(NULL, NULL, L"回合结果", L"你输掉了这一回合，受创。", NULL, TDCBF_OK_BUTTON, TD_WARNING_ICON, NULL);
                }
                if (energy <= 0) break;
            }
            if (energy > 0)
            {
                score += fightScore;
                wchar_t msg[128];
                swprintf_s(msg, L"擂台结束，获得分数 %d。当前能量: %d", fightScore, energy);
                TaskDialog(NULL, NULL, L"擂台总结", msg, NULL, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
            }
            day++;
            break;
        }
        case BTN_HACK:
        {
            // 简单的入侵小游戏：猜数字（1~3）
            TASKDIALOGCONFIG htdc = { 0 };
            htdc.cbSize = sizeof(htdc);
            htdc.hwndParent = NULL;
            htdc.dwFlags = TDF_USE_COMMAND_LINKS;
            htdc.pszWindowTitle = L"节点入侵";
            htdc.pszMainInstruction = L"黑客小游戏：猜一个节点编号（1-3）以成功入侵";
            htdc.pszContent = L"选择你认为的节点编号：";

            TASKDIALOG_BUTTON hackButtons[] = {
                { 1, L"节点 1" },
                { 2, L"节点 2" },
                { 3, L"节点 3" }
            };
            htdc.pButtons = hackButtons;
            htdc.cButtons = ARRAYSIZE(hackButtons);
            int hackChoice = 0;
            TaskDialogIndirect(&htdc, &hackChoice, NULL, NULL);

            int answer = (std::rand() % 3) + 1;
            if (hackChoice == answer)
            {
                int reward = (std::rand() % 60) + 20;
                score += reward;
                TaskDialog(NULL, NULL, L"入侵成功", L"你成功突破了节点防护，获得大量情报与资源！", NULL, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
                wchar_t msg[128];
                swprintf_s(msg, L"获得分数: +%d", reward);
                TaskDialog(NULL, NULL, L"奖励", msg, NULL, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
            }
            else
            {
                int penalty = (std::rand() % 15) + 5;
                energy -= penalty;
                TaskDialog(NULL, NULL, L"入侵失败", L"节点反制，你被追踪并消耗了能量。", NULL, TDCBF_OK_BUTTON, TD_WARNING_ICON, NULL);
                wchar_t msg[128];
                swprintf_s(msg, L"损失能量: -%d", penalty);
                TaskDialog(NULL, NULL, L"后果", msg, NULL, TDCBF_OK_BUTTON, TD_WARNING_ICON, NULL);
            }
            day++;
            break;
        }
        case BTN_EXIT:
        default:
        {
            // 退出前给出总结
            wchar_t summary[256];
            swprintf_s(summary, L"你在夜之城停留了 %d 天，最终得分为 %d，剩余能量 %d。", day - 1, score, energy);
            TaskDialog(NULL, NULL, L"冒险总结", summary, NULL, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
            running = false;
            break;
        }
        } // switch


        
            

        // 若用户仍在运行，继续下一轮
    } // while

    return 0;
}
//https://www.bilibili.com/video/BV1F2Ktz7EyC