import {defineConfig} from 'astro/config';
import starlight from '@astrojs/starlight';

export default defineConfig({
  // The preview is staged below `.build`, so bundle Astro's cookie helper
  // instead of resolving an unrelated package from a parent directory.
  vite: {ssr: {noExternal: ['cookie']}},
  integrations: [
    starlight({
      title: 'PixLite Mk3 Contact Trigger',
      description: 'Community documentation for the PixLite Mk3 Contact Trigger.',
      customCss: ['./src/styles/custom.css'],
      components: {
        Head: './src/components/Head.astro',
        Header: './src/components/Header.astro',
        Search: './src/components/Search.astro',
        SiteTitle: './src/components/SiteTitle.astro',
        SocialIcons: './src/components/HeaderActions.astro',
        ThemeSelect: './src/components/ThemeToggle.astro',
      },
      sidebar: [
        {
          label: 'Project',
          items: [
            {label: 'Project overview', slug: 'index'},
            {label: 'Contributing', slug: 'contributing'},
            {label: 'Support', slug: 'support'},
            {label: 'Security', slug: 'security'},
          ],
        },
        {
          label: 'Start here',
          items: [
            {label: 'Guide home', slug: 'guide-home'},
            {label: 'Software user guide', slug: 'user-guide'},
            {label: 'Flash with Arduino IDE', slug: 'flashing-with-arduino'},
            {label: 'Development board setup', slug: 'getting-started'},
            {label: 'Industrial 8DI setup', slug: 'getting-started-8di'},
          ],
        },
        {
          label: 'Hardware',
          items: [
            {label: 'Hardware guide', slug: 'hardware'},
            {label: 'Wiring guide', slug: 'wiring'},
            {label: 'Isolated input modules', slug: 'protected-inputs'},
            {label: 'Hardware change log', slug: 'hardware-tests'},
          ],
        },
        {
          label: 'Technical reference',
          items: [
            {label: 'Local HTTP API', slug: 'api'},
            {label: 'Architecture', slug: 'architecture'},
            {label: 'Board porting', slug: 'porting'},
          ],
        },
      ],
    }),
  ],
});
