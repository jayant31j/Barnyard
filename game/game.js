// Farm House Game (Web Version) - Simple Background Only
// Uses only barn.png as the background image

const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');

// --- Asset loading ---
const assetsPath = '../assets/'; // Adjust if needed
const images = {
  barn: new Image()
};
images.barn.src = assetsPath + 'barn.jpg'; // Place your barn/background image here

const sounds = {
  catch: new Audio(assetsPath + 'catch.mp3'), // Place your catch sound here
  miss: new Audio(assetsPath + 'miss.mp3'),   // Place your miss sound here
  gameover: new Audio(assetsPath + 'gameover.mp3') // Place your game over sound here
};

// --- Game state ---
let gameState = 'playing'; // 'playing', 'gameover'

const basket = {
  x: 400,
  y: 550,
  width: 80,
  height: 30,
  dx: 0,
  speed: 7,
  lives: 7,
  score: 0,
  gold_eggs: 0,
  silver_eggs: 0,
  reg_eggs: 0
};

// Variables to control egg count label position
let eggLabelX = 180; // default X for Gold
let eggLabelY = 34;  // default Y for all

function newEgg() {
  const kinds = ['regular', 'gold', 'silver'];
  const kind = kinds[Math.floor(Math.random() * kinds.length)];
  return {
    x: Math.random() * (800 - 75),
    y: 0,
    radius: 22,
    dy: 2.5 + Math.random() * 2.5,
    kind: kind
  };
}

let eggs = [];
const maxEggs = 3;

function resetGame() {
  basket.x = 400;
  basket.lives = 7;
  basket.score = 0;
  basket.gold_eggs = 0;
  basket.silver_eggs = 0;
  basket.reg_eggs = 0;
  eggs = [];
  for (let i = 0; i < maxEggs; i++) eggs.push(newEgg());
}

function drawBasket() {
  ctx.fillStyle = 'black';
  ctx.fillRect(basket.x, basket.y, basket.width, basket.height);
  ctx.strokeStyle = 'white';
  ctx.lineWidth = 3;
  ctx.strokeRect(basket.x, basket.y, basket.width, basket.height);
}

function drawEgg(egg) {
  ctx.save();
  ctx.beginPath();
  ctx.ellipse(egg.x, egg.y, egg.radius, egg.radius * 1.3, 0, 0, Math.PI * 2);
  ctx.fillStyle = egg.kind === 'gold' ? 'gold' : egg.kind === 'silver' ? 'silver' : 'white';
  ctx.fill();
  ctx.strokeStyle = '#888';
  ctx.lineWidth = 2;
  ctx.stroke();
  ctx.restore();
}

function drawScoreboard() {
  ctx.fillStyle = 'white';
  ctx.font = '20px sans-serif';
  ctx.fillText('Score: ' + basket.score, 20, 30);
  // Egg counts at top (with Gold and Regular swapped)
  ctx.font = '18px sans-serif';
  ctx.fillStyle = 'white';
  ctx.fillText('Regular: ' + basket.reg_eggs, eggLabelX, eggLabelY);
  ctx.fillStyle = 'silver';
  ctx.fillText('Silver: ' + basket.silver_eggs, eggLabelX + 100, eggLabelY);
  ctx.fillStyle = 'gold';
  ctx.fillText('Gold: ' + basket.gold_eggs, eggLabelX + 220, eggLabelY);
}

function drawBackground() {
  if (images.barn.complete && images.barn.naturalWidth > 0) {
    ctx.drawImage(images.barn, 0, 0, 800, 600);
  } else {
    ctx.fillStyle = '#333';
    ctx.fillRect(0, 0, 800, 600);
  }
}

function updateEggs() {
  for (let egg of eggs) {
    egg.y += egg.dy;
  }
}

function checkEggCatch() {
  for (let i = eggs.length - 1; i >= 0; i--) {
    const egg = eggs[i];
    // Check collision with basket
    if (
      egg.y + egg.radius > basket.y &&
      egg.x > basket.x &&
      egg.x < basket.x + basket.width
    ) {
      // Update score based on egg kind
      if (egg.kind === 'gold') { basket.gold_eggs++; basket.score += 3; }
      else if (egg.kind === 'silver') { basket.silver_eggs++; basket.score += 2; }
      else { basket.reg_eggs++; basket.score += 1; }
      if (sounds.catch) { try { sounds.catch.currentTime = 0; sounds.catch.play(); } catch {} }
      eggs.splice(i, 1);
      eggs.push(newEgg());
    } else if (egg.y > 600) {
      // Missed egg
      basket.lives--;
      if (sounds.miss) { try { sounds.miss.currentTime = 0; sounds.miss.play(); } catch {} }
      eggs.splice(i, 1);
      eggs.push(newEgg());
    }
  }
}

function drawStartScreen() {
  drawBackground();
  ctx.fillStyle = 'white';
  ctx.font = '48px sans-serif';
  ctx.fillText('Farm House Game', 180, 200);
  ctx.font = '28px sans-serif';
  ctx.fillText('Catch the eggs! Use arrow keys to move.', 140, 270);
  ctx.font = '24px sans-serif';
  ctx.fillText('Press SPACE to Start', 260, 350);
}

function drawGameOverScreen() {
  drawBackground();
  ctx.fillStyle = 'yellow';
  ctx.font = '48px sans-serif';
  ctx.fillText('Game Over', 270, 350);
  ctx.font = '32px sans-serif';
  ctx.fillStyle = 'white';
  ctx.fillText('Final Score: ' + basket.score, 290, 410);
  ctx.font = '24px sans-serif';
  ctx.fillText('Press R to Restart', 290, 470);
}

function draw() {
  drawBackground();
  drawBasket();
  for (let egg of eggs) drawEgg(egg);
  drawScoreboard();
}

function update() {
  basket.x += basket.dx;
  // Keep basket within bounds
  if (basket.x < 0) basket.x = 0;
  if (basket.x + basket.width > 800) basket.x = 800 - basket.width;
  updateEggs();
  checkEggCatch();
}

function gameLoop() {
  if (gameState === 'playing') {
    update();
    draw();
    if (basket.lives > 0) {
      requestAnimationFrame(gameLoop);
    } else {
      if (sounds.gameover) { try { sounds.gameover.play(); } catch {} }
      gameState = 'gameover';
      setTimeout(gameLoop, 100);
    }
  } else if (gameState === 'gameover') {
    drawGameOverScreen();
  }
}

document.addEventListener('keydown', (e) => {
  if (gameState === 'playing') {
    if (e.key === 'ArrowLeft') basket.dx = -basket.speed;
    if (e.key === 'ArrowRight') basket.dx = basket.speed;
  }
  if (gameState === 'gameover' && (e.key === 'r' || e.key === 'R')) {
    resetGame();
    gameState = 'playing';
    gameLoop();
  }
});
document.addEventListener('keyup', (e) => {
  if (e.key === 'ArrowLeft' || e.key === 'ArrowRight') basket.dx = 0;
});

resetGame();
gameLoop(); 