#include "global.h"
// 定义该函数
std::function<void(QWidget*)> repolish = [](QWidget* w){
    w->style()->unpolish(w);
    w->style()->polish(w);
};

QString gate_url_prefix = "";
