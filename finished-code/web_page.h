#ifndef WEB_PAGE_H
#define WEB_PAGE_H

const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Medicine Man</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:'Segoe UI',Tahoma,Geneva,sans-serif;min-height:100vh;background:#1a1808;overflow-x:hidden}
.page{min-height:100vh;display:none;flex-direction:column;align-items:center;position:relative;
background:#1a1808;
background-image:
 radial-gradient(ellipse at 50% -10%,rgba(139,105,20,.22) 0%,transparent 55%),
 radial-gradient(ellipse at 8% 88%,rgba(139,105,20,.25) 0%,transparent 50%),
 radial-gradient(ellipse at 92% 85%,rgba(139,105,20,.2) 0%,transparent 50%),
 radial-gradient(ellipse at 50% 100%,rgba(100,80,10,.12) 0%,transparent 35%);
overflow:hidden}
.page.active{display:flex}
.page::before{content:'';position:absolute;top:-5%;left:-18%;width:68%;height:105%;
border:7px solid rgba(139,105,20,.1);border-radius:50%;pointer-events:none}
.page::after{content:'';position:absolute;top:-8%;right:-18%;width:68%;height:105%;
border:7px solid rgba(160,120,20,.08);border-radius:50%;pointer-events:none}

.top-bar{width:100%;display:flex;align-items:center;padding:14px 16px;position:relative;z-index:10}
.back-btn{background:#D4A017;border:none;color:#fff;font-size:20px;padding:8px 16px;
border-radius:8px;cursor:pointer;line-height:1;font-weight:700;transition:background .2s}
.back-btn:hover{background:#B8860B}
.logo-wrap{position:absolute;left:50%;transform:translateX(-50%)}

.pg-num{position:absolute;bottom:10px;right:14px;color:rgba(255,255,255,.25);font-size:11px;z-index:10}

.front-title{font-size:40px;font-weight:800;color:#F5C518;margin-top:60px;text-align:center;z-index:5;
text-shadow:0 2px 8px rgba(0,0,0,.4)}
.front-btn{background:transparent;border:3px solid #F5C518;color:#F5C518;padding:14px 44px;
font-size:20px;font-weight:700;cursor:pointer;margin-top:24px;transition:all .25s;z-index:5;border-radius:4px}
.front-btn:hover{background:#F5C518;color:#1a1808}
.logo-big{width:150px;height:150px;margin-top:36px;z-index:5}

.cards{display:flex;gap:24px;margin-top:55px;flex-wrap:wrap;justify-content:center;padding:0 20px;z-index:5}
.card{width:280px;border:2px solid #D4A017;border-radius:12px;overflow:hidden;cursor:pointer;
transition:transform .2s,box-shadow .2s;background:#fff}
.card:hover{transform:scale(1.04);box-shadow:0 4px 20px rgba(212,160,23,.3)}
.card-hd{background:linear-gradient(180deg,#F5D060,#D4A017);padding:16px;text-align:center;
font-weight:700;font-size:18px;color:#333}
.card-bd{background:#D4EDAA;padding:16px;color:#444;font-size:14px;min-height:58px;line-height:1.5}

.section-title{font-size:28px;font-weight:700;font-style:italic;color:#fff;margin-top:12px;z-index:5;
text-shadow:0 2px 6px rgba(0,0,0,.3)}

.sched-box{background:#f5f5f0;border-radius:12px;padding:20px;margin-top:20px;
width:92%;max-width:640px;color:#333;z-index:5;box-shadow:0 4px 16px rgba(0,0,0,.3)}
.sched-hd{display:flex;font-weight:700;font-size:14px;padding:0 8px 10px;border-bottom:1px solid #ddd;margin-bottom:12px}
.sched-hd span:first-child{flex:1}
.med-row{display:flex;align-items:center;gap:10px;margin-bottom:12px;padding:10px;
border:1px solid #ccc;border-radius:8px;background:#fff}
.med-row input[type=text]{flex:1;padding:8px 10px;border:1px solid #ccc;border-radius:5px;font-size:14px;
font-family:inherit}
.med-row input[type=number]{width:80px;padding:8px;border:1px solid #ccc;border-radius:5px;
font-size:14px;text-align:center;font-family:inherit}
.x-btn{width:34px;height:34px;border-radius:50%;border:2px solid #7CB342;
background:linear-gradient(135deg,#F5D060,#8BC34A);color:#fff;font-size:17px;font-weight:700;
cursor:pointer;display:flex;align-items:center;justify-content:center;flex-shrink:0;transition:opacity .2s}
.x-btn:hover{opacity:.7}
.status-dot{display:inline-block;width:10px;height:10px;border-radius:50%;flex-shrink:0}
.status-dot.idle{background:#888}
.status-dot.alarm{background:#f44;box-shadow:0 0 6px #f44}
.status-dot.blinking{background:#ff0;animation:blnk .5s infinite}
@keyframes blnk{50%{opacity:.15}}

.pitch-opts{margin-top:32px;width:85%;max-width:360px;z-index:5}
.pitch-row{display:flex;align-items:center;justify-content:space-between;border:2px solid rgba(255,255,255,.7);
border-radius:10px;padding:16px 20px;margin-bottom:16px;cursor:pointer;font-size:17px;font-weight:700;
color:#fff;transition:all .2s}
.pitch-row:hover{background:rgba(255,255,255,.08);border-color:#F5C518}
.pitch-row.selected{border-color:#F5C518;background:rgba(245,197,24,.12)}
.pitch-row input[type=radio]{width:22px;height:22px;accent-color:#D4A017;cursor:pointer}

.decor{position:absolute;inset:0;pointer-events:none;overflow:hidden;z-index:1}
.dp{position:absolute;width:14px;height:30px;border-radius:7px;
background:linear-gradient(to bottom,#F5A623 50%,#7CB342 50%);opacity:.6}

.save-msg{color:#7CB342;font-size:14px;font-weight:600;margin-top:10px;min-height:20px;z-index:5}

@media(max-width:600px){
 .cards{flex-direction:column;align-items:center}
 .card{width:92%}
 .front-title{font-size:30px}
 .section-title{font-size:22px}
 .sched-box{padding:14px}
}
</style>
</head>
<body>

<!-- ========== PAGE 1 - Front ========== -->
<div id="p1" class="page active">
  <div class="decor">
    <div class="dp" style="bottom:18%;right:7%;transform:rotate(25deg)"></div>
    <div class="dp" style="bottom:28%;right:17%;transform:rotate(-18deg)"></div>
    <div class="dp" style="bottom:11%;right:30%;transform:rotate(42deg)"></div>
    <div class="dp" style="bottom:22%;left:10%;transform:rotate(-32deg)"></div>
    <div class="dp" style="bottom:14%;left:24%;transform:rotate(50deg)"></div>
    <div class="dp" style="bottom:30%;left:36%;transform:rotate(-8deg)"></div>
    <div class="dp" style="bottom:8%;right:48%;transform:rotate(18deg)"></div>
    <div class="dp" style="bottom:24%;right:55%;transform:rotate(-42deg)"></div>
  </div>
  <div class="front-title">Medicine Man</div>
  <button class="front-btn" onclick="go(2)">To website</button>
  <svg class="logo-big" viewBox="0 0 100 100">
    <path d="M22,80 C0,56 0,28 22,8" fill="none" stroke="#7CB342" stroke-width="5.5" stroke-linecap="round"/>
    <path d="M78,8 C100,32 100,60 78,80" fill="none" stroke="#F5A623" stroke-width="5.5" stroke-linecap="round"/>
    <path d="M38,50 L38,35 A12,12 0 0 1 62,35 L62,50" fill="none" stroke="#F5A623" stroke-width="3.5"/>
    <path d="M38,50 L38,65 A12,12 0 0 0 62,65 L62,50" fill="none" stroke="#7CB342" stroke-width="3.5"/>
    <line x1="38" y1="50" x2="62" y2="50" stroke="#aaa" stroke-width="1.2"/>
  </svg>
  <div class="pg-num">1</div>
</div>

<!-- ========== PAGE 2 - Main Lobby ========== -->
<div id="p2" class="page">
  <div class="decor">
    <div class="dp" style="bottom:14%;right:5%;transform:rotate(30deg)"></div>
    <div class="dp" style="bottom:24%;right:15%;transform:rotate(-22deg)"></div>
    <div class="dp" style="bottom:9%;right:28%;transform:rotate(48deg)"></div>
    <div class="dp" style="bottom:19%;right:42%;transform:rotate(-12deg)"></div>
    <div class="dp" style="bottom:30%;right:10%;transform:rotate(58deg)"></div>
    <div class="dp" style="bottom:17%;left:7%;transform:rotate(-36deg)"></div>
    <div class="dp" style="bottom:27%;left:21%;transform:rotate(14deg)"></div>
    <div class="dp" style="bottom:11%;left:34%;transform:rotate(-48deg)"></div>
  </div>
  <div class="top-bar">
    <button class="back-btn" onclick="go(1)">&larr;</button>
    <div class="logo-wrap">
      <svg width="42" height="42" viewBox="0 0 100 100">
        <path d="M22,80 C0,56 0,28 22,8" fill="none" stroke="#7CB342" stroke-width="5.5" stroke-linecap="round"/>
        <path d="M78,8 C100,32 100,60 78,80" fill="none" stroke="#F5A623" stroke-width="5.5" stroke-linecap="round"/>
        <path d="M38,50 L38,35 A12,12 0 0 1 62,35 L62,50" fill="none" stroke="#F5A623" stroke-width="3.5"/>
        <path d="M38,50 L38,65 A12,12 0 0 0 62,65 L62,50" fill="none" stroke="#7CB342" stroke-width="3.5"/>
        <line x1="38" y1="50" x2="62" y2="50" stroke="#aaa" stroke-width="1.2"/>
      </svg>
    </div>
    <div style="width:50px"></div>
  </div>
  <div class="cards">
    <div class="card" onclick="go(3)">
      <div class="card-hd">Schedule</div>
      <div class="card-bd">-Choose the amount of time between doses.</div>
    </div>
    <div class="card" onclick="go(4)">
      <div class="card-hd">Box pitch</div>
      <div class="card-bd">-Choose the type of sound your box makes.</div>
    </div>
  </div>
  <div class="pg-num">2</div>
</div>

<!-- ========== PAGE 3 - Dosage Schedule ========== -->
<div id="p3" class="page">
  <div class="decor">
    <div class="dp" style="bottom:12%;right:8%;transform:rotate(22deg)"></div>
    <div class="dp" style="bottom:22%;right:20%;transform:rotate(-28deg)"></div>
    <div class="dp" style="bottom:7%;right:36%;transform:rotate(38deg)"></div>
    <div class="dp" style="bottom:17%;left:9%;transform:rotate(-42deg)"></div>
    <div class="dp" style="bottom:26%;left:24%;transform:rotate(12deg)"></div>
    <div class="dp" style="bottom:9%;left:40%;transform:rotate(-18deg)"></div>
  </div>
  <div class="top-bar">
    <button class="back-btn" onclick="go(2)">&larr;</button>
    <div class="logo-wrap">
      <svg width="42" height="42" viewBox="0 0 100 100">
        <path d="M22,80 C0,56 0,28 22,8" fill="none" stroke="#7CB342" stroke-width="5.5" stroke-linecap="round"/>
        <path d="M78,8 C100,32 100,60 78,80" fill="none" stroke="#F5A623" stroke-width="5.5" stroke-linecap="round"/>
        <path d="M38,50 L38,35 A12,12 0 0 1 62,35 L62,50" fill="none" stroke="#F5A623" stroke-width="3.5"/>
        <path d="M38,50 L38,65 A12,12 0 0 0 62,65 L62,50" fill="none" stroke="#7CB342" stroke-width="3.5"/>
        <line x1="38" y1="50" x2="62" y2="50" stroke="#aaa" stroke-width="1.2"/>
      </svg>
    </div>
    <div style="width:50px"></div>
  </div>
  <div class="section-title">Dosage schedule</div>
  <div class="sched-box">
    <div class="sched-hd"><span>Medication</span><span>Seconds apart</span></div>
    <div class="med-row">
      <input type="text" id="n1" value="Medication1" onchange="saveComp(1)">
      <input type="number" id="t1" value="0" min="0" onchange="saveComp(1)">
      <span class="status-dot idle" id="s1" title="idle"></span>
      <button class="x-btn" onclick="clearComp(1)" title="Clear timer">&#10005;</button>
    </div>
    <div class="med-row">
      <input type="text" id="n2" value="Medication2" onchange="saveComp(2)">
      <input type="number" id="t2" value="0" min="0" onchange="saveComp(2)">
      <span class="status-dot idle" id="s2" title="idle"></span>
      <button class="x-btn" onclick="clearComp(2)" title="Clear timer">&#10005;</button>
    </div>
  </div>
  <div class="save-msg" id="smsg"></div>
  <div class="pg-num">3</div>
</div>

<!-- ========== PAGE 4 - Box Pitch ========== -->
<div id="p4" class="page">
  <div class="decor">
    <div class="dp" style="bottom:14%;right:9%;transform:rotate(32deg)"></div>
    <div class="dp" style="bottom:24%;right:24%;transform:rotate(-20deg)"></div>
    <div class="dp" style="bottom:9%;right:40%;transform:rotate(44deg)"></div>
    <div class="dp" style="bottom:19%;left:7%;transform:rotate(-34deg)"></div>
    <div class="dp" style="bottom:28%;left:22%;transform:rotate(10deg)"></div>
  </div>
  <div class="top-bar">
    <button class="back-btn" onclick="go(2)">&larr;</button>
    <div class="logo-wrap">
      <svg width="42" height="42" viewBox="0 0 100 100">
        <path d="M22,80 C0,56 0,28 22,8" fill="none" stroke="#7CB342" stroke-width="5.5" stroke-linecap="round"/>
        <path d="M78,8 C100,32 100,60 78,80" fill="none" stroke="#F5A623" stroke-width="5.5" stroke-linecap="round"/>
        <path d="M38,50 L38,35 A12,12 0 0 1 62,35 L62,50" fill="none" stroke="#F5A623" stroke-width="3.5"/>
        <path d="M38,50 L38,65 A12,12 0 0 0 62,65 L62,50" fill="none" stroke="#7CB342" stroke-width="3.5"/>
        <line x1="38" y1="50" x2="62" y2="50" stroke="#aaa" stroke-width="1.2"/>
      </svg>
    </div>
    <div style="width:50px"></div>
  </div>
  <div class="section-title">Box pitch</div>
  <div class="pitch-opts">
    <label class="pitch-row" id="pr1000" onclick="setPitch(1000)">
      <span>High</span>
      <input type="radio" name="pitch" value="1000">
    </label>
    <label class="pitch-row selected" id="pr700" onclick="setPitch(700)">
      <span>Medium</span>
      <input type="radio" name="pitch" value="700" checked>
    </label>
    <label class="pitch-row" id="pr300" onclick="setPitch(300)">
      <span>Low</span>
      <input type="radio" name="pitch" value="300">
    </label>
  </div>
  <div class="pg-num">4</div>
</div>

<script>
function go(n){
  document.querySelectorAll('.page').forEach(function(p){p.classList.remove('active')});
  document.getElementById('p'+n).classList.add('active');
  if(n===3||n===4) loadStatus();
}

function saveComp(c){
  var name=document.getElementById('n'+c).value;
  var time=document.getElementById('t'+c).value;
  if(!time||time<0) time=0;
  fetch('/api?action=setTime'+c+'&value='+time+'&name='+encodeURIComponent(name))
  .then(function(){showMsg('Saved!');}).catch(function(){showMsg('Connection error');});
}

function clearComp(c){
  document.getElementById('t'+c).value='';
  fetch('/api?action=clearTime'+c)
  .then(function(){showMsg('Cleared compartment '+c);loadStatus();})
  .catch(function(){showMsg('Connection error');});
}

function setPitch(v){
  document.querySelectorAll('.pitch-row').forEach(function(r){r.classList.remove('selected')});
  document.getElementById('pr'+v).classList.add('selected');
  document.querySelectorAll('input[name=pitch]').forEach(function(r){r.checked=(r.value==v)});
  fetch('/api?action=setPitch&value='+v).catch(function(){});
}

function showMsg(txt){
  var m=document.getElementById('smsg');
  if(m){m.textContent=txt;setTimeout(function(){m.textContent='';},2500);}
}

function loadStatus(){
  fetch('/api?action=status').then(function(r){return r.json()}).then(function(d){
    if(d.time1!==undefined) document.getElementById('t1').value=d.time1||'';
    if(d.time2!==undefined) document.getElementById('t2').value=d.time2||'';
    if(d.name1) document.getElementById('n1').value=d.name1;
    if(d.name2) document.getElementById('n2').value=d.name2;
    if(d.pitch){
      document.querySelectorAll('input[name=pitch]').forEach(function(r){r.checked=(r.value==String(d.pitch))});
      document.querySelectorAll('.pitch-row').forEach(function(r){r.classList.remove('selected')});
      var sel=document.getElementById('pr'+d.pitch);
      if(sel) sel.classList.add('selected');
    }
    if(d.state1) updateDot('s1',d.state1);
    if(d.state2) updateDot('s2',d.state2);
  }).catch(function(){});
}

function updateDot(id,state){
  var dot=document.getElementById(id);
  if(!dot) return;
  dot.className='status-dot '+state;
  dot.title=state;
}

setInterval(function(){
  if(document.getElementById('p3').classList.contains('active')) loadStatus();
},5000);

loadStatus();
</script>
</body>
</html>
)rawliteral";

#endif
