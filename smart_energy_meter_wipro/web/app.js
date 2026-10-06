const powerCanvas = document.getElementById('powerChart');
const energyCanvas = document.getElementById('energyChart');

function drawLine(canvas, values, label, unit, threshold = null) {
  const ctx = canvas.getContext('2d');
  const rect = canvas.getBoundingClientRect();
  const dpr = window.devicePixelRatio || 1;
  canvas.width = rect.width * dpr;
  canvas.height = rect.height * dpr;
  ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  const w = rect.width, h = rect.height;
  ctx.clearRect(0, 0, w, h);
  if (!values || values.length < 1) return;

  const pad = {l:48,r:18,t:18,b:30};
  const plotW = Math.max(10, w-pad.l-pad.r), plotH = Math.max(10,h-pad.t-pad.b);
  const max = Math.max(1, ...values, threshold || 0) * 1.12;

  ctx.strokeStyle = '#e7edf4'; ctx.lineWidth = 1;
  for(let i=0;i<5;i++){
    const y = pad.t + plotH*i/4;
    ctx.beginPath(); ctx.moveTo(pad.l,y); ctx.lineTo(w-pad.r,y); ctx.stroke();
    ctx.fillStyle='#7b8797'; ctx.font='11px system-ui';
    ctx.fillText((max*(1-i/4)).toFixed(label==='Power'?0:2), 5, y+4);
  }
  if (threshold !== null) {
    const y = pad.t + plotH*(1-threshold/max);
    ctx.setLineDash([6,5]); ctx.strokeStyle='#e26d4f';
    ctx.beginPath(); ctx.moveTo(pad.l,y); ctx.lineTo(w-pad.r,y); ctx.stroke(); ctx.setLineDash([]);
    ctx.fillStyle='#c4573d'; ctx.font='11px system-ui'; ctx.fillText('Threshold', w-pad.r-58, y-5);
  }

  const pts = values.map((v,i)=>{
    const x = pad.l + (values.length===1 ? plotW/2 : i*plotW/(values.length-1));
    const y = pad.t + plotH*(1-v/max);
    return [x,y];
  });
  ctx.strokeStyle='#1769aa'; ctx.lineWidth=3; ctx.lineJoin='round'; ctx.lineCap='round';
  ctx.beginPath(); pts.forEach(([x,y],i)=>i?ctx.lineTo(x,y):ctx.moveTo(x,y)); ctx.stroke();
  ctx.fillStyle='#1769aa';
  const last=pts[pts.length-1]; ctx.beginPath(); ctx.arc(last[0],last[1],4,0,Math.PI*2); ctx.fill();
  ctx.fillStyle='#6d7889'; ctx.font='11px system-ui'; ctx.fillText(unit, pad.l, h-7);
}

async function getStatus(){
  try{
    const r=await fetch('/api/status',{cache:'no-store'});
    if(!r.ok) throw new Error('HTTP '+r.status);
    const d=await r.json();

    document.getElementById('pulses').textContent=d.pulses;
    document.getElementById('kwh').textContent=Number(d.energy_kwh).toFixed(3);
    document.getElementById('wh').textContent=Number(d.energy_wh).toFixed(1)+' Wh';
    document.getElementById('power').textContent=Number(d.power_w).toFixed(0);
    document.getElementById('status').textContent=d.alert?'⚠ HIGH':'NORMAL';
    document.getElementById('threshold').textContent='Threshold: '+Number(d.threshold_w).toFixed(0)+' W';
    document.getElementById('thresholdInfo').textContent=Number(d.threshold_w).toFixed(0)+' W';
    document.getElementById('driverState').textContent=d.driver_connected?'Connected':'Simulator';
    document.getElementById('driverState').className=d.driver_connected?'connected':'simulator';
    document.getElementById('device').textContent=d.device;
    document.getElementById('mode').textContent=d.mode;
    document.getElementById('ratio').textContent=Number(d.pulses_per_kwh).toFixed(0)+' pulses/kWh';
    document.getElementById('onlineText').textContent='SYSTEM ONLINE';
    document.getElementById('onlineDot').className='dot online-dot';
    document.getElementById('status').className=d.alert?'high':'';

    drawLine(powerCanvas,d.history_power||[],'Power','W',Number(d.threshold_w));
    drawLine(energyCanvas,d.history_energy||[],'Energy','kWh');
    document.getElementById('powerEmpty').style.display=(d.history_power||[]).length?'none':'block';
    document.getElementById('energyEmpty').style.display=(d.history_energy||[]).length?'none':'block';
  }catch(e){
    document.getElementById('onlineText').textContent='SYSTEM OFFLINE';
    document.getElementById('onlineDot').className='dot offline-dot';
  }
}

async function pulse(n){
  await fetch('/api/pulse?n='+n,{cache:'no-store'});
  await getStatus();
}
async function resetMeter(){
  await fetch('/api/reset',{cache:'no-store'});
  await getStatus();
}
window.addEventListener('resize',getStatus);
getStatus();
setInterval(getStatus,1000);
