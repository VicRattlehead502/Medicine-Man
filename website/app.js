/* eslint-disable no-console */

const STORAGE = {
  account: 'dumbstruck_account',
  session: 'dumbstruck_session',
  signupDraft: 'dumbstruck_signup_draft',
  schedule: 'dumbstruck_schedule',
  contacts: 'dumbstruck_contacts',
  audioName: 'dumbstruck_audio_name',
  avatarImage: 'dumbstruck_avatar_image',
};

const ROUTES = {
  loggedout: 'loggedout', // image 5
  login: 'login', // image 8
  signup: 'signup', // image 7
  dashboard: 'dashboard', // image 2
  profile: 'profile', // image 1
  sound: 'sound', // image 6
  schedule: 'schedule', // image 3
  contacts: 'contacts', // image 4
};

const state = {
  scheduleItems: [],
  contacts: [],
};

function loadJson(key, fallback) {
  try {
    const raw = localStorage.getItem(key);
    if (!raw) return fallback;
    return JSON.parse(raw);
  } catch {
    return fallback;
  }
}

function saveJson(key, value) {
  localStorage.setItem(key, JSON.stringify(value));
}

function getAccount() {
  return loadJson(STORAGE.account, null);
}

function isLoggedIn() {
  const s = loadJson(STORAGE.session, null);
  return Boolean(s && s.loggedIn);
}

function hasAccount() {
  return Boolean(getAccount());
}

function setSession(loggedIn) {
  if (!loggedIn) {
    localStorage.removeItem(STORAGE.session);
    return;
  }
  saveJson(STORAGE.session, { loggedIn: true, at: Date.now() });
}

function currentRoute() {
  const h = (location.hash || '').replace(/^#\//, '');
  if (!h) return ROUTES.dashboard;
  const seg = h.split('/')[0];
  const allowed = Object.values(ROUTES);
  return allowed.includes(seg) ? seg : ROUTES.dashboard;
}

function navigate(route) {
  location.hash = `#/${route}`;
}

function showOnlyPage(pageId) {
  const pages = document.querySelectorAll('.page');
  for (const p of pages) {
    const isTarget = p.id === `page-${pageId}`;
    p.setAttribute('aria-hidden', String(!isTarget));
  }
}

function requireAuth(targetRoute) {
  if (!hasAccount() || !isLoggedIn()) {
    // Match spec: Account buttons send them to "not logged in" first.
    navigate(ROUTES.loggedout);
    return false;
  }
  // If there is an account but session is missing, also treat as logged out.
  return targetRoute;
}

function renderRoute() {
  const route = currentRoute();

  // Only protected pages require auth; dashboard is always accessible.
  if ([ROUTES.profile, ROUTES.sound, ROUTES.schedule, ROUTES.contacts].includes(route)) {
    if (!requireAuth(route)) return;
  }

  // Render specific page
  showOnlyPage(route);

  updateAccountLabel();

  if (route === ROUTES.profile) renderProfile();
  if (route === ROUTES.schedule) renderSchedule();
  if (route === ROUTES.contacts) renderContacts();
  if (route === ROUTES.sound) renderSound();

  if (route === ROUTES.login) renderLogin();
  if (route === ROUTES.signup) renderSignup();
}

function updateAccountLabel(nameOverride) {
  const labelName = (() => {
    if (typeof nameOverride === 'string') return nameOverride.trim();
    const acc = getAccount();
    return (acc && typeof acc.name === 'string') ? acc.name.trim() : '';
  })();
  const finalText = labelName ? labelName : 'Account';
  const els = document.querySelectorAll('[data-account-text]');
  for (const el of els) el.textContent = finalText;
}

function initNavHandlers() {
  document.addEventListener('click', (e) => {
    const btn = e.target && e.target.closest ? e.target.closest('[data-nav]') : null;
    const nav = btn && btn.getAttribute ? btn.getAttribute('data-nav') : null;

    if (btn && nav === 'menu') {
      // Menu: go to dashboard (image 2).
      navigate(ROUTES.dashboard);
      return;
    }

    if (btn && nav === 'account') {
      // Button with "2": go to profile (image 1) if logged in, otherwise page 5.
      if (hasAccount() && isLoggedIn()) navigate(ROUTES.profile);
      else navigate(ROUTES.loggedout);
      return;
    }

    const goLogin = e.target && e.target.closest ? e.target.closest('[data-action="go-login"]') : null;
    if (goLogin) {
      navigate(ROUTES.login);
      return;
    }

    const goSignup = e.target && e.target.closest ? e.target.closest('[data-action="go-signup"]') : null;
    if (goSignup) {
      navigate(ROUTES.signup);
      return;
    }

    const routeBtn = e.target && e.target.closest ? e.target.closest('[data-route]') : null;
    if (routeBtn) {
      const r = routeBtn.getAttribute('data-route');
      if (!r) return;
      if ([ROUTES.profile, ROUTES.sound, ROUTES.schedule, ROUTES.contacts].includes(r)) {
        if (!requireAuth(r)) return;
      }
      navigate(r);
    }
  });
}

function wireForms() {
  const loginForm = document.getElementById('login-form');
  const signupForm = document.getElementById('signup-form');
  const profileForm = document.getElementById('profile-form');

  loginForm?.addEventListener('submit', (e) => {
    e.preventDefault();
    const form = new FormData(loginForm);
    const name = String(form.get('name') || '').trim();
    const password = String(form.get('password') || '').trim();

    const acc = getAccount();
    if (!acc || acc.name !== name || acc.password !== password) {
      alert('Invalid login. Make sure your Name and Password match your saved account.');
      return;
    }

    setSession(true);
    navigate(ROUTES.profile);
  });

  // Signup page: remember everything user types.
  const signupFields = Array.from(signupForm.querySelectorAll('input, textarea'));
  for (const el of signupFields) {
    el.addEventListener('input', () => {
      const fd = new FormData(signupForm);
      const draft = {
        name: String(fd.get('name') || ''),
        password: String(fd.get('password') || ''),
        conditions: String(fd.get('conditions') || ''),
      };
      saveJson(STORAGE.signupDraft, draft);
      updateAccountLabel(draft.name);
    });
  }

  signupForm?.addEventListener('submit', (e) => {
    e.preventDefault();
    const fd = new FormData(signupForm);
    const name = String(fd.get('name') || '').trim();
    const password = String(fd.get('password') || '').trim();
    const conditions = String(fd.get('conditions') || '').trim();

    const acc = { name, password, conditions };
    saveJson(STORAGE.account, acc);
    setSession(true);

    // Keep the draft as-is (so it "remembers all info" even later).
    navigate(ROUTES.profile);
  });

  // Profile: autosave edits to name/conditions.
  const profileInputs = profileForm ? Array.from(profileForm.querySelectorAll('input, textarea')) : [];
  for (const el of profileInputs) {
    el.addEventListener('input', () => {
      const acc = getAccount() || { name: '', password: '', conditions: '' };
      const fd = new FormData(profileForm);
      acc.name = String(fd.get('name') || '');
      acc.conditions = String(fd.get('conditions') || '');
      saveJson(STORAGE.account, acc);
    });
  }
}

function renderLogin() {
  const loginForm = document.getElementById('login-form');
  const nameInput = loginForm?.querySelector('input[name="name"]');
  const acc = getAccount();
  if (acc && nameInput) {
    nameInput.value = acc.name || '';
  }
  // Password left blank (typical).
}

function renderSignup() {
  const signupForm = document.getElementById('signup-form');
  const fd = new FormData(signupForm);
  const draft = loadJson(STORAGE.signupDraft, null) || {};

  const nameEl = signupForm.querySelector('input[name="name"]');
  const passEl = signupForm.querySelector('input[name="password"]');
  const condEl = signupForm.querySelector('textarea[name="conditions"]');

  if (nameEl) nameEl.value = draft.name ?? '';
  if (passEl) passEl.value = draft.password ?? '';
  if (condEl) condEl.value = draft.conditions ?? '';

  // If draft is empty, seed from existing account to make it feel "remembered".
  if (!draft.name && !draft.password && !draft.conditions) {
    const acc = getAccount();
    if (acc) {
      if (nameEl) nameEl.value = acc.name || '';
      if (passEl) passEl.value = acc.password || '';
      if (condEl) condEl.value = acc.conditions || '';
    }
  }
}

function renderProfile() {
  const profileForm = document.getElementById('profile-form');
  const acc = getAccount();
  if (!profileForm || !acc) return;
  const fd = new FormData(profileForm);
  const nameEl = profileForm.querySelector('input[name="name"]');
  const condEl = profileForm.querySelector('textarea[name="conditions"]');
  if (nameEl) nameEl.value = acc.name || '';
  if (condEl) condEl.value = acc.conditions || '';

  renderAvatar();
}

function renderAvatar() {
  const avatarPreview = document.getElementById('avatar-preview');
  const avatarFile = document.getElementById('avatar-file');
  const xCircle = document.querySelector('.x-circle');
  if (!avatarPreview || !xCircle) return;

  const dataUrl = localStorage.getItem(STORAGE.avatarImage);
  if (dataUrl) {
    avatarPreview.src = dataUrl;
    avatarPreview.hidden = false;
    xCircle.hidden = true;
  } else {
    avatarPreview.hidden = true;
    avatarPreview.removeAttribute('src');
    xCircle.hidden = false;
  }

  // Keep input reset if needed.
  if (avatarFile && avatarFile.value) {
    // no-op; browser may block setting value for security, but preview is from localStorage anyway.
  }
}

function wireAvatarUpload() {
  const btn = document.getElementById('avatar-add');
  const fileInput = document.getElementById('avatar-file');
  if (!btn || !fileInput) return;

  btn.addEventListener('click', () => fileInput.click());

  fileInput.addEventListener('change', () => {
    const file = fileInput.files && fileInput.files[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = () => {
      const dataUrl = String(reader.result || '');
      if (!dataUrl) return;
      localStorage.setItem(STORAGE.avatarImage, dataUrl);
      renderAvatar();
    };
    reader.readAsDataURL(file);
  });
}

function defaultSchedule() {
  return [
    { name: 'Medication1', time: '09:30' },
    { name: 'Medication2', time: '16:30' }
  ];
}

function defaultContacts() {
  return ['+359...', '+359...'];
}

function loadScheduleState() {
  const items = loadJson(STORAGE.schedule, null);
  if (!Array.isArray(items) || items.length === 0) return defaultSchedule();
  // Normalize shape
  return items.map((it) => ({
    name: String(it?.name ?? ''),
    time: String(it?.time ?? '09:30'),
  }));
}

function loadContactsState() {
  const items = loadJson(STORAGE.contacts, null);
  if (!Array.isArray(items) || items.length === 0) return defaultContacts();
  return items.map((p) => String(p ?? ''));
}

function saveScheduleState() {
  saveJson(STORAGE.schedule, state.scheduleItems);
}

function saveContactsState() {
  saveJson(STORAGE.contacts, state.contacts);
}

function renderSchedule() {
  state.scheduleItems = loadScheduleState();
  const rowsEl = document.getElementById('schedule-rows');
  if (!rowsEl) return;

  rowsEl.innerHTML = '';
  state.scheduleItems.forEach((item, idx) => {
    const row = document.createElement('div');
    row.className = 'row';

    const nameWrap = document.createElement('div');
    nameWrap.style.display = 'flex';
    nameWrap.style.alignItems = 'center';
    nameWrap.style.gap = '10px';

    const nameInput = document.createElement('input');
    nameInput.type = 'text';
    nameInput.value = item.name;
    nameInput.placeholder = 'Medication name';
    nameInput.style.flex = '1';
    nameInput.style.width = 'auto';
    nameInput.addEventListener('input', () => {
      state.scheduleItems[idx].name = nameInput.value;
      saveScheduleState();
    });

    nameWrap.appendChild(nameInput);

    const timeInput = document.createElement('input');
    timeInput.type = 'time';
    timeInput.readOnly = false;
    timeInput.step = '60';
    timeInput.value = item.time || '09:30';
    const syncTime = () => {
      state.scheduleItems[idx].time = timeInput.value;
      saveScheduleState();
    };
    timeInput.addEventListener('input', syncTime);
    timeInput.addEventListener('change', syncTime);

    const removeBtn = document.createElement('button');
    removeBtn.type = 'button';
    removeBtn.className = 'icon-circle';
    removeBtn.setAttribute('aria-label', `Remove schedule row ${idx + 1}`);
    removeBtn.innerHTML = '<span class="icon-circle-x" aria-hidden="true"></span>';
    removeBtn.addEventListener('click', () => {
      if (state.scheduleItems.length <= 1) return;
      state.scheduleItems.splice(idx, 1);
      saveScheduleState();
      renderSchedule();
    });

    row.appendChild(nameWrap);
    row.appendChild(timeInput);
    row.appendChild(removeBtn);

    rowsEl.appendChild(row);
  });
}

function renderContacts() {
  state.contacts = loadContactsState();
  const rowsEl = document.getElementById('contacts-rows');
  if (!rowsEl) return;

  rowsEl.innerHTML = '';
  state.contacts.forEach((phone, idx) => {
    const row = document.createElement('div');
    row.className = 'row row-contacts';

    const phoneWrap = document.createElement('div');
    phoneWrap.style.position = 'relative';

    const input = document.createElement('input');
    input.className = 'text-input phone-input';
    input.type = 'text';
    input.value = phone;
    input.placeholder = '+359...';
    input.addEventListener('input', () => {
      state.contacts[idx] = input.value;
      saveContactsState();
    });

    phoneWrap.appendChild(input);

    const removeBtn = document.createElement('button');
    removeBtn.type = 'button';
    removeBtn.className = 'icon-circle';
    removeBtn.setAttribute('aria-label', `Remove contact row ${idx + 1}`);
    removeBtn.innerHTML = '<span class="icon-circle-x" aria-hidden="true"></span>';
    removeBtn.addEventListener('click', () => {
      if (state.contacts.length <= 1) return;
      state.contacts.splice(idx, 1);
      saveContactsState();
      renderContacts();
    });

    row.appendChild(phoneWrap);
    row.appendChild(removeBtn);
    rowsEl.appendChild(row);
  });
}

function wireDynamicSchedule() {
  const addBtn = document.getElementById('schedule-add');
  addBtn?.addEventListener('click', () => {
    state.scheduleItems = loadScheduleState();
    state.scheduleItems.push({ name: '', time: '' });
    saveScheduleState();
    renderSchedule();
  });
}

function wireDynamicContacts() {
  const addBtn = document.getElementById('contacts-add');
  addBtn?.addEventListener('click', () => {
    state.contacts = loadContactsState();
    state.contacts.push('');
    saveContactsState();
    renderContacts();
  });
}

function renderSound() {
  const input = document.getElementById('audio-name');
  const name = localStorage.getItem(STORAGE.audioName) || '';
  if (input) input.value = name;
}

function wireSound() {
  const fileInput = document.getElementById('audio-file');
  const changeBtn = document.getElementById('audio-change');
  const clearBtn = document.getElementById('audio-clear');
  const playBtn = document.getElementById('audio-play');
  const audioName = document.getElementById('audio-name');
  const audioPlayer = document.getElementById('audio-player');

  // Keep a reference so we can revoke object URLs and stop leaks.
  let audioObjectUrl = null;

  changeBtn?.addEventListener('click', () => fileInput?.click());

  fileInput?.addEventListener('change', () => {
    const file = fileInput.files && fileInput.files[0];
    const name = file ? file.name : '';
    localStorage.setItem(STORAGE.audioName, name);
    if (audioName) audioName.value = name;

    if (!audioPlayer) return;

    // Replace previously selected audio.
    try {
      if (audioObjectUrl) URL.revokeObjectURL(audioObjectUrl);
    } catch {
      // ignore
    }
    audioObjectUrl = null;

    if (file) {
      audioObjectUrl = URL.createObjectURL(file);
      audioPlayer.src = audioObjectUrl;
      audioPlayer.load();
    } else {
      audioPlayer.pause();
      audioPlayer.removeAttribute('src');
      audioPlayer.load();
    }
  });

  clearBtn?.addEventListener('click', () => {
    localStorage.removeItem(STORAGE.audioName);
    if (audioName) audioName.value = '';

    if (audioPlayer) {
      audioPlayer.pause();
      try {
        if (audioObjectUrl) URL.revokeObjectURL(audioObjectUrl);
      } catch {
        // ignore
      }
      audioObjectUrl = null;
      audioPlayer.removeAttribute('src');
      audioPlayer.load();
    }
  });

  playBtn?.addEventListener('click', async () => {
    if (!audioPlayer || !audioPlayer.src) return;
    try {
      await audioPlayer.play();
    } catch {
      // Some browsers require a user gesture; click should count, but keep UX safe.
      alert('Could not play audio. Try selecting a file again.');
    }
  });
}

function wireProfileFinish() {
  const btn = document.getElementById('profile-finish');
  btn?.addEventListener('click', () => {
    navigate(ROUTES.dashboard);
  });
}

function boot() {
  // Initialize default state lazily, only when needed.
  state.scheduleItems = [];
  state.contacts = [];

  initNavHandlers();
  wireForms();
  wireDynamicSchedule();
  wireDynamicContacts();
  wireSound();
  wireProfileFinish();
  wireAvatarUpload();

  window.addEventListener('hashchange', renderRoute);

  // Ensure at least one aria-hidden flag exists.
  for (const p of document.querySelectorAll('.page')) {
    p.setAttribute('aria-hidden', 'true');
  }

  // Render initial view
  if (!location.hash) navigate(ROUTES.dashboard);
  renderRoute();
}

document.addEventListener('DOMContentLoaded', boot);

