const root = document.documentElement;
const themeButton = document.querySelector('[data-theme-toggle]');
const mobileNavigation = document.querySelector('[data-mobile-navigation]');

root.dataset.theme = localStorage.getItem('advatekDocsTheme') || 'dark';

themeButton?.addEventListener('click', () => {
  const next = root.dataset.theme === 'light' ? 'dark' : 'light';
  root.dataset.theme = next;
  localStorage.setItem('advatekDocsTheme', next);
  themeButton.setAttribute('aria-label', `Use ${next === 'light' ? 'dark' : 'light'} theme`);
});

document.querySelector('[data-print]')?.addEventListener('click', () => {
  window.print();
});

mobileNavigation?.addEventListener('change', event => {
  const destination = event.target.value;
  if (destination) window.location.href = destination;
});
